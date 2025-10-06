#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>
#include "batchHandler.h"

extern bool wifiConnecting;
extern unsigned long wifiConnectStart;

void connectToESPAccessPointAsync();
void sendDataToESP32(std::vector<SensorData>& batch);
bool postToESP32(std::vector<SensorData>& batch);
void retryFailedBatches();
void updateLogger();

#endif // WIFIHANDLER_H