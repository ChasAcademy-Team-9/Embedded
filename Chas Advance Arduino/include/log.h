#ifndef LOG_H
#define LOG_H

#include "arduinoLogger.h"

enum ErrorType {
    NONE = 0,
    TOO_LOW = 1,
    TOO_HIGH = 2,
    SENSOR_FAIL = 4
};


void logEvent(String eventType, String description, String status);
void logSensorData(float temperature, float humidity, ErrorType errorType);
void logStartup();

#endif
