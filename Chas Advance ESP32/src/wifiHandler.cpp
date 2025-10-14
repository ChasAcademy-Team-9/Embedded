#include "wifiHandler.h"
#include "ESPSECRETS.h"
#include "espLogger.h"
#include <queue>
#include <mutex>
#include <HTTPClient.h>

struct IncomingBatch
{
  std::vector<uint8_t> data;
  uint32_t clientIP;
};

// Global queue and mutex for handling incoming batches
std::queue<IncomingBatch> batchQueue;
std::mutex queueMutex;

unsigned long timeSinceDataReceived = 0;
WiFiServer server;
extern ESPLogger logger;

// API batch send variables
unsigned long lastRetryTime = 0;
const unsigned long retryInterval = 10000;
bool retryInProgress = false;

void initWifi()
{
  connectToWiFi();
  setupAccessPoint();
  setupHttpServer();

  // Set up NTP time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void connectToWiFi()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(sta_ssid, sta_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // Connect to WiFi
  Serial.println("\nESP32 connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupAccessPoint()
{
  // Set up Access Point for Arduino to connect to
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("ESP32 AP started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void setupHttpServer()
{
  // Define route
  server.begin(80);
  Serial.println("HTTP server started");
}

// --- Helpers ---

// Check that the request line starts with "POST /data"
// If not, return 404 and close connection
bool isValidPostRequest(WiFiClient &client, const String &requestLine)
{
  if (!requestLine.startsWith("POST /data"))
  {
    client.println("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
    client.stop();
    return false;
  }
  return true;
}

// Read HTTP headers and extract the Content-Length value
// Returns -1 if not found
int readContentLength(WiFiClient &client)
{
  int contentLength = -1;
  while (client.connected())
  {
    String line = client.readStringUntil('\n');
    line.trim();
    if (line.length() == 0)
      break; // empty line means end of headers
    if (line.startsWith("Content-Length:"))
    {
      contentLength = line.substring(15).toInt();
    }
  }
  return contentLength;
}

// Read the binary body of the request into a buffer
// Returns true if exactly contentLength bytes were read
bool readRequestBody(WiFiClient &client, std::vector<uint8_t> &buffer, int contentLength)
{
  buffer.resize(contentLength);
  int bytesRead = 0;
  unsigned long start = millis();

  // Keep reading until all bytes received or 3s timeout
  while (bytesRead < contentLength && millis() - start < 3000)
  {
    if (client.available())
    {
      bytesRead += client.read(buffer.data() + bytesRead, contentLength - bytesRead);
    }
  }
  return bytesRead == contentLength;
}

// Parse the binary buffer into sendMillis + SensorData entries
// Returns false if the payload is invalid
bool parseBatch(const std::vector<uint8_t> &buffer, uint32_t &sendMillis, std::vector<SensorData> &batch)
{
  if (buffer.size() < sizeof(sendMillis))
    return false;

  // Extract sendMillis (first 4 bytes)
  memcpy(&sendMillis, buffer.data(), sizeof(sendMillis));

  // Remaining data should be a multiple of SensorData struct size
  size_t dataSize = buffer.size() - sizeof(sendMillis);
  if (dataSize % sizeof(SensorData) != 0)
    return false;

  // Copy SensorData entries into batch vector
  batch.resize(dataSize / sizeof(SensorData));
  memcpy(batch.data(), buffer.data() + sizeof(sendMillis), dataSize);
  return true;
}

// Send an HTTP response and close the connection
// 200 = OK, anything else = Bad Request
void respond(WiFiClient &client, int code)
{
  if (code == 200)
  {
    client.println("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
  }
  else
  {
    client.printf("HTTP/1.1 %d Bad Request\r\nConnection: close\r\n\r\n", code);
  }
  client.stop();
}

void handleClientAsync()
{
  WiFiClient client = server.available();
  if (!client)
    return;

  // Step 1: Read request line
  String requestLine = client.readStringUntil('\n');
  requestLine.trim();
  if (!isValidPostRequest(client, requestLine))
    return;

  // Step 2: Read headers and Content-Length
  int contentLength = readContentLength(client);
  if (contentLength <= 0 || contentLength > 10 * 1024)
  {
    respond(client, 400);
    return;
  }

  // Step 3: Read binary body
  std::vector<uint8_t> buffer;
  if (!readRequestBody(client, buffer, contentLength))
  {
    respond(client, 400);
    return;
  }

  // Step 4: Push to processing queue
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    batchQueue.push({buffer, client.remoteIP()});
  }

  // Step 5: Immediate response
  respond(client, 200);
}

// --- Batch processing task ---
void processBatches(void *parameter)
{
  while (true)
  {
    IncomingBatch batch;
    bool hasBatch = false;

    {
      std::lock_guard<std::mutex> lock(queueMutex);
      if (!batchQueue.empty())
      {
        batch = batchQueue.front();
        batchQueue.pop();
        hasBatch = true;
      }
    }

    if (hasBatch)
    {
      // Parse, assign timestamps, log to LittleFS, post to server
      uint32_t sendMillis;
      std::vector<SensorData> sensorBatch;
      if (parseBatch(batch.data, sendMillis, sensorBatch))
      {
        assignAbsoluteTimestamps(sendMillis, sensorBatch);
        Serial.printf("Received batch with %d entries\n", sensorBatch.size());
        for (const auto &entry : sensorBatch)
        {
          logSensorData(formatUnixTime(entry.timestamp), entry.temperature, entry.humidity, static_cast<ErrorType>(entry.errorType));
        }
        if (!postBatchToServer(sensorBatch, -1))
        { // -1 means not from flash
          Serial.println("Failed to send batch to backend server - saving in flash");
          logger.logBatch(sensorBatch);
          uint16_t batchIndex = 0;
          if (logger.getNewestBatch(sensorBatch, batchIndex))
          {
            logger.logSendStatus(batchIndex, false, "Failed send");
          }
        }
        else
        {
          Serial.println("Batch received from sensor was sent successfully to backend server");
        }
      }
    }
    else
    {
      vTaskDelay(10 / portTICK_PERIOD_MS); // small delay if queue empty
    }
  }
}

void trySendPendingBatches()
{
  static std::vector<SensorData> batchEntries;
  static uint16_t batchIndex = 0;
  static int attempt = 0;

  if (!retryInProgress)
  {
    if (!logger.getOldestBatch(batchEntries, batchIndex))
    {
      return;
    }
    attempt = 0;
    retryInProgress = true;
  }

  if (millis() - lastRetryTime < retryInterval)
    return;
  lastRetryTime = millis();
  attempt++;

  Serial.printf("Sending saved batch %d attempt %d/%d...\n", batchIndex, attempt, 3);

  if (postBatchToServer(batchEntries, batchIndex))
  {
    Serial.println("Saved batch sent successfully, removing file");
    String fname = logger.getBatchFilename(batchIndex);
    LittleFS.remove(fname);
    retryInProgress = false;
    batchEntries.clear();
    return;
  }

  if (attempt >= 3)
  {
    Serial.println("Sending saved batch failed after 3 retries, will retry later");
    logger.logSendStatus(batchIndex, false, "3 retries failed");
    retryInProgress = false;
  }
}

// Send a batch to API - Example POST HTTP Request
bool sendJsonToServer(const String &jsonString, int batchId)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected - can't send batch to database!");
    logger.logSendStatus(batchId, false, "WiFi not connected");
    return false;
  }

  HTTPClient http;
  http.begin(host); // HTTPS URL
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0)
  {
    String payload = http.getString(); // response from server
    if (httpResponseCode == 201)
    {
      logger.logSendStatus(batchId, true, "OK");
      Serial.println("Batch sent successfully!");
      http.end();
      return true;
    }
    else
    {
      logger.logSendStatus(batchId, false, payload);
      Serial.print("Server responded with code: ");
      Serial.println(httpResponseCode);
      http.end();
      return false;
    }
  }
  else
  {
    Serial.print("Error sending POST: ");
    Serial.println(httpResponseCode);
    logger.logSendStatus(batchId, false, "Connection failed");
    http.end();
    return false;
  }
}

// High-level function to post a batch
bool postBatchToServer(const std::vector<SensorData> &batch, int batchId)
{
  if (batch.empty())
  {
    Serial.println("Batch is empty, not sending.");
    return false;
  }

  String jsonString = serializeBatchToJson(batch);
  return sendJsonToServer(jsonString, batchId);
}