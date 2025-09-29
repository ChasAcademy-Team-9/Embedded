#include <Arduino.h>
#include "mockJson.h"
#include "wifiHandler.h"
#include "espLogger.h"

ESPLogger logger;

void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("Starting ESP32...");

  //Initialize logger
  logger.begin();
  logStartup();

  // Print all previous log entries
  logger.printBatches();
  logger.printErrors();

  initWifi();
}

void loop()
{
  checkDataTimeout(timeSinceDataReceived);
  server.handleClient();
  trySendPendingBatches();
  delay(1000);
}

