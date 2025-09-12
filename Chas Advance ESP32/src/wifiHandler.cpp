#include "wifiHandler.h"
#include "jsonParser.h"
#include "ESPSECRETS.h"

void WifiHandler::init()
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

  // Set up Access Point for Arduino to connect to
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("ESP32 AP started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Define route
  server.on("/data", HTTP_POST, [&]()
            { handlePostRequest(); });

  server.begin(80);
  Serial.println("HTTP server started");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

/* Function to handle POST requests to /data
    It reads the JSON body, parses it, and logs the sensor data.*/
void WifiHandler::handlePostRequest()
{
  if (server.hasArg("plain"))
  { // "plain" contains POST body
    String body = server.arg("plain");

    StaticJsonDocument<250> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error)
    {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      server.send(400, "text/plain", "Bad JSON");
      return;
    }

    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      doc["received_timestamp"] = "TIME_ERROR";
    }
    else
    {
      char buffer[20];
      strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);

      String timestamp = String(buffer);

      // Add to JSON
      doc["timestamp"] = timestamp;
    }

    String updatedBody;
    serializeJson(doc, updatedBody);

    // Parse sensor data
    parseJson(updatedBody);

    server.send(200, "text/plain", "OK");
    timeSinceDataRecevied = millis();
  }
  else
  {
    server.send(400, "text/plain", "No data received");
  }
}