#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>
#include "batchHandler.h"

extern bool wifiConnecting;
extern unsigned long wifiConnectStart;
extern uint32_t currentESPTime;

void connectToESPAccessPointAsync();
bool attemptSendBatch();
void sendDataToESP32(std::vector<SensorData> &batch);
bool postToESP32(std::vector<SensorData> &batch);
void updateLogger();
uint32_t getTimeFromESP32();
void updateCurrentESPTime();

#endif // WIFIHANDLER_H