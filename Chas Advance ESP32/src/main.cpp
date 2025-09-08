#include <Arduino.h>
#include "log.h"
#include "jsonParser.h"
#include "mockJson.h"
#include <WiFi.h>
#include <WebServer.h>


const char *ssid = "Chas Academy";
const char *password = "EverythingLouderThanEverythingElse";

WiFiServer server(80); // Server listen to port 80

void setup()
{
  Serial.begin(115200);
  logStartup();
  delay(6000);

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
}

void loop()
{
  String json = generateMockJson();
  parseJson(json);
  delay(2000);
}

