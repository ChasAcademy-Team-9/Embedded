#include "wifiHandler.h"
#include "ESPSECRETS.h"
#include "espLogger.h"

unsigned long timeSinceDataReceived = 0;
WebServer server;
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
  server.on("/data", HTTP_POST, [&]()
            { handlePostRequest(); });

  server.begin(80);
  Serial.println("HTTP server started");
}

/* Function to handle POST requests to /data
    It reads the JSON body, parses it, and logs the sensor data.*/
void handlePostRequest()
{
  if (server.hasArg("plain"))
  { // "plain" contains POST body
    String body = server.arg("plain");

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error)
    {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      server.send(400, "text/plain", "Bad JSON");
      return;
    }

    if (!doc.is<JsonArray>())
    {
      server.send(400, "text/plain", "Expected JSON array");
      return;
    }
    JsonArray arr = doc.as<JsonArray>();
    // Send batch to API
    if (!postToServer(arr))
    {
      // failed - Log to flash
      Serial.print("Failed to send new batch to server - saving in flash \n");
      logger.logBatch(arr);
    }

    parseJsonArray(arr, getTimeStamp());
    server.send(200, "text/plain", "OK");
    timeSinceDataReceived = millis();
  }
  else
  {
    server.send(400, "text/plain", "No data received");
  }
}

void trySendPendingBatches()
{
  static std::vector<SensorEntry> batchEntries;
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

  // Convert to JSON
  StaticJsonDocument<2048> doc;
  doc["timestamp"] = formatUnixTime(batchEntries[0].timestamp);
  JsonArray dataArr = doc.createNestedArray("data");
  for (const auto &entry : batchEntries)
  {
    JsonObject obj = dataArr.createNestedObject();
    obj["t"] = entry.temperature;
    obj["h"] = entry.humidity;
    obj["err"] = entry.error;
    obj["et"] = entry.errorType;
  }

  if (postToServer(dataArr))
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
bool postToServer(JsonArray &arr)
{
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("Connection to server failed");
    return false;
  }

  StaticJsonDocument<1024> doc;
  doc["timestamp"] = getTimeStamp();
  doc["data"] = arr;

  String jsonString;
  serializeJson(doc, jsonString);

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