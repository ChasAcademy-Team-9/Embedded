#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

enum ErrorType {
    NONE = 0,
    TOO_LOW = 1,
    TOO_HIGH = 2,
    SENSOR_FAIL = 4
};


extern const char *ntpServer;
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const int dataReceivedThreshold = 70000; // 70 seconds

void logEvent(String timestamp, String eventType, String description, String status);
void logSensorData(String timestamp, float temperature, float humidity, ErrorType errorType);
void logStartup();
void checkDataTimeout(unsigned long &timeSinceDataReceived);
String getTimeStamp();

#endif