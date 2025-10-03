#include <Arduino.h>
#include "mockJson.h"
#include "wifiHandler.h"
#include "espLogger.h"

Logger logger;

void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("Starting ESP32...");

  //Initialize logger
  logger.begin();
  logStartup();

  // Print all previous log entries
  logger.printAll();

  initWifi();
}

void loop()
{
  checkDataTimeout(timeSinceDataReceived);

  server.handleClient();
  delay(1000);
}

