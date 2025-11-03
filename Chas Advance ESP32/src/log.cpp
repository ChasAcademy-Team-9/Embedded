#include "log.h"

/// Default NTP server hostname used by the system (definition for extern)
const char *ntpServer = "pool.ntp.org";

/**
 * @brief Print a composed event line to Serial.
 *
 * Format: "<timestamp> <eventType> <description> <status>"
 */
void logEvent(String timestamp, String eventType, String description, String status)
{
    Serial.print(timestamp);
    Serial.print(" ");
    Serial.print(eventType);
    Serial.print(" ");
    Serial.print(description);
    Serial.print(" ");
    Serial.println(status);
}

/**
 * @brief Format sensor values and emit via logEvent().
 *
 * Maps errorType to a suitable event level and message.
 */
void logSensorData(String timestamp, float temperature, float humidity, ErrorType errorType)
{
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Temp=%.1f Hum=%.1f", temperature, humidity);

    switch (errorType)
    {
    case NONE:
        logEvent(timestamp, "INFO", buffer, "OK");
        break;
    case TOO_LOW:
        logEvent(timestamp, "\033[31mWARNING\033[0m Sensor data too low", buffer, "CHECK");
        break;
    case TOO_HIGH:
        logEvent(timestamp, "\033[31mWARNING\033[0m Sensor data too high", buffer, "CHECK");
        break;
    case SENSOR_FAIL:
        logEvent(timestamp, "\033[31mERROR\033[0m", "Sensor failure", "FAIL");
        break;
    default:
        logEvent(timestamp, "INFO", buffer, "UNKNOWN");
        break;
    }
}

/**
 * @brief Emit a startup/system reset message with current timestamp.
 *
 * Uses getTimeStamp() to obtain current local time. Intended to be called
 * from setup() after time/NTP is configured.
 */
void logStartup()
{
    String timeStamp = getTimeStamp();
    logEvent(timeStamp, "SYSTEM", "RESET", "\033[32mOK\033[0m");
}

/**
 * @brief Check for prolonged absence of sensor data and log if needed.
 *
 * If the difference between current millis() and timeSinceDataReceived
 * exceeds dataReceivedThreshold an error event is emitted and the
 * timeSinceDataReceived is reset to the current millis() value to avoid
 * repeated spamming of the same message.
 *
 * @param timeSinceDataReceived Reference to last-received millis value
 */
void checkDataTimeout(unsigned long &timeSinceDataReceived)
{
    if ((millis() - timeSinceDataReceived) > dataReceivedThreshold)
    {
        // Generate warning if no data received for the configured threshold
        logEvent(getTimeStamp(), "\033[31mERROR\033[0m", "No data received for " + String(dataReceivedThreshold / 1000) + " seconds", "FAIL");
        timeSinceDataReceived = millis(); // Reset timer
    }
}

/**
 * @brief Obtain a formatted timestamp string using NTP
 *
 * Attempts to read the current local time via getLocalTime(). On failure
 * prints a diagnostic and returns "TIME_ERROR".
 * 
 * @return Timestamp in "YYYY-MM-DD HH:MM:SS" format or "TIME_ERROR".
 */
String getTimeStamp()
{
    String timeStamp = "\033[31mTIME_ERROR\033[0m";

    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("\033[31mFailed to obtain time\033[0m");
        return timeStamp;
    }
    else
    {
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        timeStamp = String(buffer);
    }
    return timeStamp;
}

/**
 * @brief Convert unix epoch seconds to a human-readable timestamp.
 *
 * Uses localtime() to produce a formatted string.
 *
 * @param ts Unix epoch seconds
 * @return Formatted "YYYY-MM-DD HH:MM:SS" string
 */
String formatUnixTime(uint32_t ts)
{
    time_t t = ts;
    struct tm *timeinfo = localtime(&t); // UTC
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}

/**
 * @brief Parse a human-readable timestamp into unix epoch seconds.
 *
 * Expected input format: "YYYY-MM-DD HH:MM:SS".
 * On parse failure prints a diagnostic and returns 0.
 *
 * @param tsStr Timestamp string to parse
 * @return Unix epoch seconds or 0 on failure
 */
uint32_t timestampStringToUnix(const String &tsStr)
{
    struct tm timeinfo = {0};

    if (sscanf(tsStr.c_str(), "%4d-%2d-%2d %2d:%2d:%2d",
               &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday,
               &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec) != 6)
    {
        Serial.println("\033[31mFailed to parse timestamp\033[0m");
        return 0;
    }

    timeinfo.tm_year -= 1900; // struct tm expects years since 1900
    timeinfo.tm_mon -= 1;     // struct tm months are 0-11

    return (uint32_t)mktime(&timeinfo);
}

/**
 * @brief Convert sensor-relative millis timestamps into absolute unix times.
 *
 * Sensors may deliver timestamps as their internal millis() values; this
 * function uses the host's current unix time and the provided sendMillis
 * (sensor millis when batch was sent) to compute absolute times
 * for each entry. The function modifies batch[i].timestamp to hold unix
 * epoch seconds for each measurement.
 *
 * Algorithm summary:
 *  - Query current unix time (now) via getTimeStamp()
 *  - Compute delay between last measurement's sensor-millis and sendMillis
 *  - Compute unix time for latest measurement as now - delay_seconds
 *  - Walk backwards converting prior entries using their millis deltas
 *
 * @param sendMillis 32-bit send time in milliseconds
 * @param batch Vector of SensorData entries whose timestamp fields are updated
 */
void assignAbsoluteTimestamps(uint32_t sendMillis, std::vector<SensorData> &batch)
{
  if (batch.empty())
    return;

  // ESP32 current Unix time (seconds)
  uint32_t now = timestampStringToUnix(getTimeStamp());

  // Last measurement
  SensorData &latest = batch.back();
  uint32_t lastMeasurementMillis = latest.timestamp; // Arduino millis of last measurement

  // How long since last measurement until batch was sent (ms)
  uint32_t delayMs = (sendMillis >= lastMeasurementMillis) ? (sendMillis - lastMeasurementMillis) : 0;

  // Absolute Unix time of last measurement
  uint32_t lastMeasurementUnix = now - (delayMs / 1000);
  latest.timestamp = lastMeasurementUnix;

  // Walk backwards for previous entries and convert using measured deltas
  for (size_t i = batch.size() - 2; i < batch.size(); --i)
  {
    uint32_t deltaMs = lastMeasurementMillis - batch[i].timestamp; // millis between measurements
    batch[i].timestamp = lastMeasurementUnix - (deltaMs / 1000);
    if (i == 0) break;
  }
}