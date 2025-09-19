#ifndef LOG_H
#define LOG_H
#include "espLogger.h"


extern const char *ntpServer;
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void logEvent(String timestamp, String eventType, String description, String status);
void logSensorData(String timestamp, float temperature, float humidity, bool error);
void logStartup();
void checkDataTimeout(unsigned long &timeSinceDataReceived);
String getTimeStamp();

#endif