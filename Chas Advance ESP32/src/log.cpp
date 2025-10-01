#include "log.h"

const char *ntpServer = "pool.ntp.org";

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

void logSensorData(String timestamp, float temperature, float humidity, ErrorType errorType)
{
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Temp=%.1f Hum=%.1f", temperature, humidity);

    switch (errorType)
    {
    case NONE:
        logEvent(timestamp,"INFO", buffer, "OK");
        break;
    case TOO_LOW:
        logEvent(timestamp,"WARNING_Sensor data too low", buffer, "CHECK");
        break;
    case TOO_HIGH:
        logEvent(timestamp,"WARNING_Sensor data too high", buffer, "CHECK");
        break;
    case SENSOR_FAIL:
        logEvent(timestamp,"ERROR", "Sensor failure", "FAIL");
    }
}

// OBS - Innehåller mockdata
void logStartup()
{
    String timeStamp = getTimeStamp();
    logEvent(timeStamp, "SYSTEM", "RESET", "OK");
}

void checkDataTimeout(unsigned long &timeSinceDataReceived)
{
    if ((millis() - timeSinceDataReceived) > dataReceivedThreshold)
    {
        // Generate warning if no data received for the configured threshold
        logEvent(getTimeStamp(), "ERROR", "No data received for " + String(dataReceivedThreshold / 1000) + " seconds", "FAIL");
        timeSinceDataReceived = millis(); // Reset timer
    }
}

String getTimeStamp()
{
    String timeStamp = "TIME_ERROR";

    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
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

//Convert unix timestamp to formatted string
String formatUnixTime(uint32_t ts)
{
    time_t t = ts;
    struct tm *timeinfo = localtime(&t);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}
