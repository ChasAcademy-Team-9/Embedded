#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>

extern bool wifiConnecting;
extern unsigned long wifiConnectStart;

void connectToESPAccessPointAsync();
void sendDataToESP32(String jsonString);
void updateLogger();

#endif // WIFIHANDLER_H