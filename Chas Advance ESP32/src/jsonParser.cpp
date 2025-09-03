#include <ArduinoJson.h>
#include "jsonParser.h"

void parseJson(String json)
{
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error)
    {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        delay(2000);
        return;
    }

    String timestamp = doc["timestamp"] | "";
    float temperature = doc["temperature"] | 0.0;
    float humidity = doc["humidity"] | 0.0;
    bool success = doc["success"] | false;

    logSensorData(timestamp, temperature, humidity, success);
}