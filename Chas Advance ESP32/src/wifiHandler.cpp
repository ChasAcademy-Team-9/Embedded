#include "wifiHandler.h"
#include "ESPSECRETS.h"

unsigned long timeSinceDataReceived = 0;
WebServer server;

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

    parseJsonArray(doc.as<JsonArray>(), getTimeStamp());
    server.send(200, "text/plain", "OK");
    timeSinceDataReceived = millis();
  }
  else
  {
    server.send(400, "text/plain", "No data received");
  }
}
