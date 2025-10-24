#ifndef LOG_H
#define LOG_H

#include <Arduino.h>
#include "sensorData.h"

/* @brief Log.h - Simple logging and time utilities.
 *
 * This module provides Serial-based logging helpers, timestamp
 * formatting/parsing and a helper to convert sensor-local millis-based
 * timestamps into absolute UNIX time values.
 */

/// Types of sensor error flags (bitmask-friendly where applicable)
enum ErrorType {
    NONE = 0,      /**< No error */
    TOO_LOW = 1,   /**< Sensor reading below expected range */
    TOO_HIGH = 2,  /**< Sensor reading above expected range */
    SENSOR_FAIL = 4/**< Sensor failure / invalid readings */
};

/// NTP server hostname (defined in log.cpp)
extern const char *ntpServer;

/// Time zone / NTP offsets (seconds)
const long gmtOffset_sec = 3600;      /**< GMT offset in seconds */
const int daylightOffset_sec = 3600;  /**< Daylight saving offset in seconds */

/// Threshold (ms) after which "no data" is considered an error (70s)
const int dataReceivedThreshold = 70000; // 70 seconds

/**
 * @brief Log a generic system event to Serial.
 *
 * This is a lightweight helper that emits a single line composed of the
 * provided timestamp, event type, description and status.
 *
 * @param timestamp Human-readable timestamp (YYYY-MM-DD HH:MM:SS)
 * @param eventType Short event category (e.g. "SYSTEM", "ERROR", "INFO")
 * @param description Free-form description text
 * @param status Short status string (e.g. "OK", "FAIL")
 */
void logEvent(String timestamp, String eventType, String description, String status);

/**
 * @brief Log a sensor reading to Serial in a compact format.
 *
 * Formats temperature/humidity together with provided timestamp and an
 * ErrorType to indicate sensor status.
 *
 * @param timestamp Human-readable timestamp
 * @param temperature Temperature value (Celsius)
 * @param humidity Relative humidity (percent)
 * @param errorType Enumerated error type for the measurement
 */
void logSensorData(String timestamp, float temperature, float humidity, ErrorType errorType);

/**
 * @brief Emit startup banner / system reset log line.
 *
 * Uses getTimeStamp() to print current time and a "SYSTEM RESET" style message.
 */
void logStartup();

/**
 * @brief Check whether data has been received recently and emit an alert.
 *
 * Compares current millis() to the provided timeSinceDataReceived value and,
 * if the threshold is exceeded, logs an error and resets the timer.
 *
 * @param timeSinceDataReceived Reference to the last-received millis timestamp
 */
void checkDataTimeout(unsigned long &timeSinceDataReceived);

/**
 * @brief Obtain a formatted current timestamp using NTP.
 *
 * The function tries to read the local time via getLocalTime(). If time
 * is unavailable the returned string will be "TIME_ERROR".
 *
 * @return Formatted timestamp "YYYY-MM-DD HH:MM:SS" or "TIME_ERROR"
 */
String getTimeStamp();

/**
 * @brief Convert a unix epoch (seconds) to a human-readable timestamp.
 *
 * @param ts Unix epoch time in seconds
 * @return Formatted timestamp "YYYY-MM-DD HH:MM:SS"
 */
String formatUnixTime(uint32_t ts);

/**
 * @brief Parse a timestamp string into a unix epoch (seconds).
 *
 * Expects input formatted as "YYYY-MM-DD HH:MM:SS". On parse failure this
 * returns 0 and prints a diagnostic to Serial.
 *
 * @param tsStr Timestamp string to parse
 * @return Unix epoch seconds (or 0 on failure)
 */
uint32_t timestampStringToUnix(const String &tsStr);

/**
 * @brief Convert a relative sendMillis and a batch of SensorData (with
 * Arduino-style millis timestamps) into absolute unix timestamps.
 *
 * The sensor sends a sendMillis and each SensorData contains a 
 * timestamp field that is sensor-local millis.
 * This function:
 *  - queries the current unix time from getTimeStamp(),
 *  - computes the elapsed time between the last measurement and sendMillis,
 *  - rewrites each SensorData.timestamp to an absolute unix timestamp (seconds).
 *
 * Note: modifies the provided batch in-place.
 *
 * @param sendMillis 32-bit send time in millis (as received from sensor)
 * @param batch Vector of SensorData whose timestamp fields are adjusted
 */
void assignAbsoluteTimestamps(uint32_t sendMillis, std::vector<SensorData> &batch);

#endif // LOG_H