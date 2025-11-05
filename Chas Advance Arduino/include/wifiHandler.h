#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>
#include "batchHandler.h"

extern bool isWifiConnecting;
extern unsigned long wifiConnectStartMillis;
extern uint32_t currentESPTime;
extern bool isTimeInitialized;

void connectToESPAccessPointAsync();
bool attemptSendBatch();
bool sendDataToESP32(std::vector<SensorData> &batch);
bool postToESP32(std::vector<SensorData> &batch);
void updateLogger();
bool getTimeFromESP32();
void updateCurrentESPTime();

#endif // WIFIHANDLER_H