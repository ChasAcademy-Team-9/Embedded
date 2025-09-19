#include "log.h"

extern Logger logger;

void logEvent(String eventType, String description, String status)
{
    Serial.print(eventType);
    Serial.print(" ");
    Serial.print(description);
    Serial.print(" ");
    Serial.println(status);
    logger.log(eventType + " " + description + " " + status);

}

void logSensorData(float temperature, float humidity, bool error)
{
    if (error)
    {
        logEvent("ERROR", "No data", "FAIL");
    }
    else
    {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Temp=%.1f Hum=%.1f", temperature, humidity);
        logEvent("INFO", buffer, "OK");
    }
}

void logStartup()
{
    logEvent("SYSTEM", "RESET", "OK");
}