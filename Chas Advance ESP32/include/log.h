#ifndef LOG_H
#define LOG_H
#include <Arduino.h>

void logEvent(String timestamp, String eventType, String description, String status);
void logSensorData(String timestamp, float temperature, float humidity, bool error);
// void logStartup();

#endif