#include <Arduino.h>
#include "log.h"
#include "jsonParser.h"
#include "mockJson.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "DataReceiver.h"

const char *ssid = "Chas Academy";
const char *password = "EverythingLouderThanEverythingElse";

WebServer server(80); // Server listen to port 80

void setup()
{
  Serial.begin(115200);
  logStartup();
  delay(5000);

  Serial.println("Starting ESP32...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nESP32 connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Define route
  server.on("/data", HTTP_POST, [&]()
            { HandlePostRequest(server); });

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
  delay(1000);
}

