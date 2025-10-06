#include "log.h"

void logEvent(String eventType, String description, String status)
{

    Serial.print(eventType);
    Serial.print(" ");
    Serial.print(description);
    Serial.print(" ");
    Serial.println(status);
}

void logSensorData(float temperature, float humidity, ErrorType errorType)
{
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Temp=%.1f Hum=%.1f", temperature, humidity);

    switch (errorType)
    {
    case NONE:
        logEvent("INFO", buffer, "OK");
        break;
    case TOO_LOW:
        logEvent("WARNING_Sensor data too low", buffer, "CHECK");
        break;
    case TOO_HIGH:
        logEvent("WARNING_Sensor data too high", buffer, "CHECK");
        break;
    case SENSOR_FAIL:
        logEvent("ERROR", "Sensor failure", "FAIL");
        break;
    default:
        logEvent("ERROR", "Unknown error type", "FAIL");
        break;
    }
}

void logStartup()
{
    logEvent("SYSTEM", "RESET", "OK");
}