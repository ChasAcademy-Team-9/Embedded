#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

enum ErrorType {
    NONE = 0,
    TOO_LOW = 1,
    TOO_HIGH = 2,
    SENSOR_FAIL = 4,
    WiFi_FAIL = 5
};


void logEvent(String timeStamp, String eventType, String description, String status);
void logSensorData(String timeStamp, float temperature, float humidity, ErrorType errorType);
void logStartup();

/** 
 * @brief Format a Unix timestamp (seconds since epoch) into a human-readable string
 * @param ts Unix timestamp in seconds
 * @return Formatted time string "YYYY-MM-DD HH:MM:SS" 
 * 
*/
String formatUnixTime(uint32_t ts);
#endif
