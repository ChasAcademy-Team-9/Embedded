#include <Arduino.h>
#include "mockJson.h"
#include "wifiHandler.h"
#include "espLogger.h"
#include "esp_system.h"

ESPLogger logger;

void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("Starting ESP32...");
  Serial.print("Reset reason: ");
  Serial.println(esp_reset_reason());

  //Initialize logger
  logger.begin();
  logStartup();
  //logger.clearBatches(); // For testing, clear old batches

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

