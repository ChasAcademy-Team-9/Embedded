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

// Read raw POST data into a buffer
bool readPostData(WebServer &server, std::vector<uint8_t> &buffer)
{
  int contentLength = server.arg("plain").length();
  if (contentLength == 0)
  {
    Serial.println("No POST data received");
    return false;
  }

  buffer.resize(contentLength);
  memcpy(buffer.data(), server.arg("plain").c_str(), contentLength);
  return true;
}

// Deserialize raw bytes into a vector of SensorData
bool parseSensorBatch(const std::vector<uint8_t> &buffer, std::vector<SensorData> &batch)
{
  if (buffer.size() < sizeof(uint16_t))
  {
    Serial.println("Data too short to contain count");
    return false;
  }

  uint16_t count;
  memcpy(&count, buffer.data(), sizeof(count));

  if (buffer.size() != sizeof(count) + count * sizeof(SensorData))
  {
    Serial.println("Mismatch between count and data size");
    return false;
  }

  batch.resize(count);
  memcpy(batch.data(), buffer.data() + sizeof(count), count * sizeof(SensorData));
  return true;
}

// Main handler
void handlePostRequestBinary(WiFiClient &client)
{
    if (!client.connected())
    {
        Serial.println("No client connected");
        return;
    }

    // --- Step 1: Read request line (e.g. "POST /data HTTP/1.1")
    String requestLine = client.readStringUntil('\n');
    requestLine.trim();
    Serial.println("Request: " + requestLine);

    if (!requestLine.startsWith("POST /data"))
    {
        client.println("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
        client.stop();
        return;
    }

    // --- Step 2: Read headers until blank line
    int contentLength = -1;
    while (client.connected())
    {
        String line = client.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) break; // end of headers

        if (line.startsWith("Content-Length:"))
        {
            contentLength = line.substring(15).toInt();
        }
    }

    if (contentLength <= 0)
    {
        Serial.println("No Content-Length or invalid");
        client.println("HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
        client.stop();
        return;
    }

    // --- Step 3: Read binary body
    std::vector<uint8_t> buffer(contentLength);
    int bytesRead = 0;
    unsigned long start = millis();

    while (bytesRead < contentLength && millis() - start < 3000)
    {
        if (client.available())
        {
            bytesRead += client.read(buffer.data() + bytesRead, contentLength - bytesRead);
        }
    }

    if (bytesRead != contentLength)
    {
        Serial.printf("Expected %d bytes but got %d\n", contentLength, bytesRead);
        client.println("HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
        client.stop();
        return;
    }

    // --- Step 4: Parse sendMillis + SensorData
    if (contentLength < sizeof(uint32_t))
    {
        Serial.println("Content too short for sendMillis");
        client.println("HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
        client.stop();
        return;
    }

    uint32_t sendMillis;
    memcpy(&sendMillis, buffer.data(), sizeof(sendMillis));

    size_t dataSize = contentLength - sizeof(sendMillis);
    if (dataSize % sizeof(SensorData) != 0)
    {
        Serial.println("Payload size is not a multiple of SensorData");
        client.println("HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
        client.stop();
        return;
    }

    std::vector<SensorData> batch(dataSize / sizeof(SensorData));
    memcpy(batch.data(), buffer.data() + sizeof(sendMillis), dataSize);

    // --- Step 5: Assign absolute timestamps using sendMillis
    assignAbsoluteTimestamps(sendMillis, batch);

    Serial.printf("Received batch with %d entries\n", batch.size());
    for (const auto &entry : batch)
    {
        logSensorData(formatUnixTime(entry.timestamp),
                      entry.temperature,
                      entry.humidity,
                      static_cast<ErrorType>(entry.errorType));
    }

    timeSinceDataReceived = 0; // Reset timeout counter

    // --- Step 6: Send HTTP response
    client.println("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
    client.stop();

    // --- Step 7: Attempt to send batch to backend server
    if (!postBatchToServer(batch))
    {
        Serial.println("Failed to send batch to backend server - saving in flash");
        logger.logBatch(batch);
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

  if (postBatchToServer(batchEntries))
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
    Serial.println("Batch send failed, will retry later");
    retryInProgress = false;
  }
}

// Send a batch to API - Example POST HTTP Request
bool sendJsonToServer(const String &jsonString)
{
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("Connection to server failed");
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
        client.stop();
        return true;
      }
    }
  }

  client.stop();
  Serial.println("No valid response from server");
  return false;
}

// High-level function to post a batch
bool postBatchToServer(const std::vector<SensorData> &batch)
{
  if (batch.empty())
  {
    Serial.println("Batch is empty, not sending.");
    return false;
  }

  String jsonString = serializeBatchToJson(batch);
  return sendJsonToServer(jsonString);
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