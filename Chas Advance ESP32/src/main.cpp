#include <Arduino.h>
#include "mockJson.h"
#include "wifiHandler.h"

void setup()
{
  Serial.begin(115200);
  logStartup();
  delay(3000);

  Serial.println("Starting ESP32...");
  initWifi();
}

void loop()
{
  checkDataTimeout(timeSinceDataReceived);

  server.handleClient();
  delay(1000);
}

