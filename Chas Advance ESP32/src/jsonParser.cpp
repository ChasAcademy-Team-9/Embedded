#include <ArduinoJson.h>
#include "jsonParser.h"

void parseJson(String json)
{
    StaticJsonDocument<200> doc;
    DeserializationError dError = deserializeJson(doc, json);

    if (dError)
    {
        Serial.print("JSON parse error: ");
        Serial.println(dError.c_str());
        delay(2000);
        return;
    }

    String timestamp = doc["timestamp"] | "";
    float temperature = doc["temperature"] | 0.0;
    float humidity = doc["humidity"] | 0.0;
    bool error = doc["error"] | false;

    logSensorData(timestamp, temperature, humidity, error);
}