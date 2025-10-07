#include "wifiHandler.h"
#include "ESPSECRETS.h"
#include "espLogger.h"

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
bool isValidPostRequest(WiFiClient &client, const String &requestLine) {
    if (!requestLine.startsWith("POST /data")) {
        client.println("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
        client.stop();
        return false;
    }
    return true;
}

// Read HTTP headers and extract the Content-Length value
// Returns -1 if not found
int readContentLength(WiFiClient &client) {
    int contentLength = -1;
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) break; // empty line means end of headers
        if (line.startsWith("Content-Length:")) {
            contentLength = line.substring(15).toInt();
        }
    }
    return contentLength;
}

// Read the binary body of the request into a buffer
// Returns true if exactly contentLength bytes were read
bool readRequestBody(WiFiClient &client, std::vector<uint8_t> &buffer, int contentLength) {
    buffer.resize(contentLength);
    int bytesRead = 0;
    unsigned long start = millis();

    // Keep reading until all bytes received or 3s timeout
    while (bytesRead < contentLength && millis() - start < 3000) {
        if (client.available()) {
            bytesRead += client.read(buffer.data() + bytesRead, contentLength - bytesRead);
        }
    }
    return bytesRead == contentLength;
}

// Parse the binary buffer into sendMillis + SensorData entries
// Returns false if the payload is invalid
bool parseBatch(const std::vector<uint8_t> &buffer, uint32_t &sendMillis, std::vector<SensorData> &batch) {
    if (buffer.size() < sizeof(sendMillis)) return false;

    // Extract sendMillis (first 4 bytes)
    memcpy(&sendMillis, buffer.data(), sizeof(sendMillis));

    // Remaining data should be a multiple of SensorData struct size
    size_t dataSize = buffer.size() - sizeof(sendMillis);
    if (dataSize % sizeof(SensorData) != 0) return false;

    // Copy SensorData entries into batch vector
    batch.resize(dataSize / sizeof(SensorData));
    memcpy(batch.data(), buffer.data() + sizeof(sendMillis), dataSize);
    return true;
}

// Send an HTTP response and close the connection
// 200 = OK, anything else = Bad Request
void respond(WiFiClient &client, int code) {
    if (code == 200) {
        client.println("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
    } else {
        client.printf("HTTP/1.1 %d Bad Request\r\nConnection: close\r\n\r\n", code);
    }
    client.stop();
}

// --- Main handler ---

// Handle an incoming HTTP POST request with binary body
void handlePostRequestBinary(WiFiClient &client) {
    if (!client.connected()) return;

    // Step 1: Read and validate the request line
    String requestLine = client.readStringUntil('\n');
    requestLine.trim();
    if (!isValidPostRequest(client, requestLine)) return;

    // Step 2: Read headers and extract Content-Length
    int contentLength = readContentLength(client);
    if (contentLength <= 0) { respond(client, 400); return; }

    // Step 3: Read the binary body into buffer
    std::vector<uint8_t> buffer;
    if (!readRequestBody(client, buffer, contentLength)) { respond(client, 400); return; }

    // Step 4: Parse sendMillis + SensorData entries
    uint32_t sendMillis;
    std::vector<SensorData> batch;
    if (!parseBatch(buffer, sendMillis, batch)) { respond(client, 400); return; }

    // Step 5: Add timestamps and log entries
    assignAbsoluteTimestamps(sendMillis, batch);
    Serial.printf("Received batch with %d entries\n", batch.size());
    for (const auto &entry : batch) {
        logSensorData(formatUnixTime(entry.timestamp), entry.temperature, entry.humidity, static_cast<ErrorType>(entry.errorType));
    }

    timeSinceDataReceived = 0; // reset watchdog
    respond(client, 200);      // Step 6: Send OK back to client

    // Step 7: Try to forward batch to backend, otherwise store in flash
    if (!postBatchToServer(batch,-1)) { // -1 means not from flash
        Serial.println("Failed to send batch to backend server - saving in flash");
        logger.logBatch(batch);
        uint16_t batchIndex = 0;
        if(logger.getNewestBatch(batch, batchIndex))
        {
          logger.logSendStatus(batchIndex, false, "Failed send");
        }
    }
    else {
        Serial.println("Batch received from sensor sent successfully to backend server");
    }
}

void handleClient()
{
  WiFiClient client = server.available();
  if (client)
  {
    handlePostRequestBinary(client);
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
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("Connection to server failed when sending batch");
    logger.logSendStatus(batchId, false, "Connection failed");
    return false;
  }

  client.print(String("POST /data HTTP/1.1\r\n") +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + jsonString.length() + "\r\n" +
               "Connection: close\r\n\r\n" +
               jsonString);

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 2000)
  {
    if (client.available())
    {
      String line = client.readStringUntil('\n');
      if (line.startsWith("HTTP/1.1 200"))
      {
        logger.logSendStatus(batchId, true, "OK");
        client.stop();
        return true;
      }
    }
  }

  client.stop();
  logger.logSendStatus(batchId, false, "No response");
  Serial.println("No valid response from server");
  return false;
}

// High-level function to post a batch
bool postBatchToServer(const std::vector<SensorData> &batch, int batchID)
{
  if (batch.empty())
  {
    Serial.println("Batch is empty, not sending.");
    return false;
  }

  String jsonString = serializeBatchToJson(batch);
  return sendJsonToServer(jsonString, batchID);
}

void assignAbsoluteTimestamps(uint32_t sendMillis, std::vector<SensorData> &batch)
{
    if (batch.empty()) return;

    // ESP32 current Unix time (seconds)
    uint32_t now = timestampStringToUnix(getTimeStamp());

    // Last measurement
    SensorData &latest = batch.back();
    uint32_t lastMeasurementMillis = latest.timestamp; // Arduino millis of last measurement

    // How long since last measurement until batch was sent
    uint32_t delayMs = (sendMillis >= lastMeasurementMillis) ? (sendMillis - lastMeasurementMillis) : 0;

    // Absolute Unix time of last measurement
    uint32_t lastMeasurementUnix = now - (delayMs / 1000);
    latest.timestamp = lastMeasurementUnix;

    // Walk backwards for previous entries
    for (int i = batch.size() - 2; i >= 0; i--)
    {
        uint32_t deltaMs = lastMeasurementMillis - batch[i].timestamp; // millis between measurements
        batch[i].timestamp = lastMeasurementUnix - (deltaMs / 1000);
    }
}