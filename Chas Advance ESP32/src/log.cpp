#include "log.h"

const char *ntpServer = "pool.ntp.org";
extern Logger logger;

void logEvent(String timestamp, String eventType, String description, String status)
{
    Serial.print(timestamp);
    Serial.print(" ");
    Serial.print(eventType);
    Serial.print(" ");
    Serial.print(description);
    Serial.print(" ");
    Serial.println(status);
    logger.log(timestamp + " " + eventType + " " + description + " " + status);

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

// OBS - Innehåller mockdata
void logStartup()
{
    char buf[32];
    //convert getTimeStamp() to char array
    String ts = getTimeStamp();
    const char* cstr = ts.c_str();

    snprintf(buf, sizeof(buf), cstr, random(0, 24), random(0, 60), random(0, 60));
    logEvent(buf, "SYSTEM", "RESET", "OK");
}

void checkDataTimeout(unsigned long &timeSinceDataReceived)
{
    if ((millis() - timeSinceDataReceived) > 10000)
    {
        // If no data received for 5 seconds, generate warning
        logEvent(getTimeStamp(), "ERROR", "No data recevied for 10 seconds", "FAIL");
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