#include <Arduino.h>
#include "log.h"
#include "jsonParser.h"
#include "mockJson.h"
#include <WiFi.h>
#include "wifiHandler.h"

WifiHandler wifiHandler;
void setup()
{
  Serial.begin(115200);
  logStartup();
  delay(3000);

  Serial.println("Starting ESP32...");
  wifiHandler.init();
}

void loop()
{
  wifiHandler.server.handleClient();
  Serial.print(".");
  delay(1000);
}

