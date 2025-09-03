#include "log.h"

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

void logSensorData(String timestamp, float temperature, float humidity, bool error)
{
    if (error)
    {
        logEvent(timestamp, "ERROR", "No data", "FAIL");
    }
    else
    {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Temp=%.1f Hum=%.1f", temperature, humidity);
        logEvent(timestamp, "INFO", buffer, "OK");
    }
}

// void logStartup()
// {
//     // Exempel på startup-loggning
//     logEvent("SYSTEM", "RESET", "OK");
// }