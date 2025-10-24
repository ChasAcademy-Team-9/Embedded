#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>
#include "batchHandler.h"

extern bool wifiConnecting;
extern unsigned long wifiConnectStart;

void connectToESPAccessPointAsync();
bool attemptSendBatch();
bool sendDataToESP32(std::vector<SensorData> &batch);
bool postToESP32(std::vector<SensorData> &batch);
void updateLogger();

#endif // WIFIHANDLER_H