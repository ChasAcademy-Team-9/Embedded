#include <Arduino.h>
#include "wifiHandler.h"
#include "espLogger.h"
#include "batchNetworkHandler.h"
#include "batchProcessor.h"
#include "esp_system.h"

ESPLogger logger;
int stackSize = 8192;

void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("Starting ESP32...");
  Serial.print("Reset reason: ");
  Serial.println(esp_reset_reason());

  initWifi();

  // Initialize logger
  logger.begin();
  logStartup();
  //logger.clearBatches(); // For testing, clear old batches

  // Print all previous log entries
  logger.printBatches();
  logger.printErrors();
  logger.printSendStatusLogs();
  //logger.clearSendStatusLogs(); // For testing, clear old send status logs

  xTaskCreate(processBatches, "ProcessBatches", stackSize, NULL, 1, NULL);
}

void loop()
{
  checkDataTimeout(timeSinceDataReceived);
  handleClientAsync();
  trySendPendingBatches();
}
