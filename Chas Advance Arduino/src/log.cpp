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
        logEvent(timeStamp, "WARNING_Sensor data too low", buffer, "CHECK");
        break;
    case TOO_HIGH:
        logEvent(timeStamp, "WARNING_Sensor data too high", buffer, "CHECK");
        break;
    case SENSOR_FAIL:
        logEvent(timeStamp, "ERROR", "Sensor failure", "FAIL");
        break;
    default:
        logEvent(timeStamp, "ERROR", "Unknown error type", "FAIL");
        break;
    }
}

void logStartup()
{
    logEvent("","SYSTEM", "RESET", "OK"); //Pass empty timestamp since not available at startup
}

String formatUnixTime(uint32_t ts)
{
    time_t t = ts;
    struct tm *timeinfo = localtime(&t); // UTC
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}
