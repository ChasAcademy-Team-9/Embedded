#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

extern const char *ntpServer;
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const int dataReceivedThreshold = 70000; // 70 seconds

void logEvent(String timestamp, String eventType, String description, String status);
void logSensorData(String timestamp, float temperature, float humidity, bool error);
void logStartup();
void checkDataTimeout(unsigned long &timeSinceDataReceived);
String getTimeStamp();

#endif