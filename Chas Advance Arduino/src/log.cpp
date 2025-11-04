#include "log.h"
#include <time.h>

void logEvent(String timeStamp, String eventType, String description, String status)
{
    Serial.print(timeStamp);
    Serial.print(" ");
    Serial.print(eventType);
    Serial.print(" ");
    Serial.print(description);
    Serial.print(" ");
    Serial.println(status);
}

void logSensorData(String timeStamp, float temperature, float humidity, ErrorType errorType)
{
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Temp=%.1f Hum=%.1f", temperature, humidity);

    switch (errorType)
    {
    case NONE:
        logEvent(timeStamp, "INFO", buffer, "OK");
        break;
    case TOO_LOW:
        logEvent(timeStamp, "\033[31mWARNING\033[0m Sensor data too low", buffer, "CHECK");
        break;
    case TOO_HIGH:
        logEvent(timeStamp, "\033[31mWARNING\033[0m Sensor data too high", buffer, "CHECK");
        break;
    case SENSOR_FAIL:
        logEvent(timeStamp, "\033[31mERROR\033[0m", "Sensor failure", "FAIL");
        break;
    default:
        logEvent(timeStamp, "\033[31mERROR\033[0m", "Unknown error type", "FAIL");
        break;
    }
}

void logStartup()
{
    logEvent("", "SYSTEM", "RESET", "\033[32mOK\033[0m"); // Pass empty timestamp since not available at startup
}

String formatUnixTime(uint32_t ts)
{
    const long offset = 3600;        // +1 hour offset 
    time_t t = ts + offset;           
    struct tm *timeinfo = localtime(&t); // Local time
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}
