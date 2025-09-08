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

void logStartup()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "2025-09-03 %02d:%02d:%02d", random(0, 24), random(0, 60), random(0, 60));
    logEvent(buf, "SYSTEM", "RESET", "OK");
}