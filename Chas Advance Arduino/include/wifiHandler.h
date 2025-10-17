#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>
#include "batchHandler.h"

extern bool wifiConnecting;
extern unsigned long wifiConnectStart;

void connectToESPAccessPointAsync();
bool attemptSendBatch();
void sendDataToESP32(std::vector<SensorData> &batch);
bool postToESP32(std::vector<SensorData> &batch);
void updateLogger();

// New functions for flash memory data transfer
void checkAndSendFlashData();

#endif // WIFIHANDLER_H