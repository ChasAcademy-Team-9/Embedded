#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiS3.h>
#include <ArduinoJson.h>

void connectToWiFi();
void connectToESP(String jsonString);

#endif // WIFIHANDLER_H