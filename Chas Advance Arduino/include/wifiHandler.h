#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>

void connectToESPAccessPoint();
void sendDataToESP32(String jsonString);
void updateLogger();

#endif // WIFIHANDLER_H