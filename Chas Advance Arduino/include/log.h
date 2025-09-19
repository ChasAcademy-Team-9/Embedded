#ifndef LOG_H
#define LOG_H

#include <timeProvider.h>
#include "arduinoLogger.h"

void logEvent(String eventType, String description, String status);
void logSensorData(float temperature, float humidity, bool error);
void logStartup();

#endif
