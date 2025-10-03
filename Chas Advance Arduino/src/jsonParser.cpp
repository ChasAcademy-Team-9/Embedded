#include "jsonParser.h"

String parseJSON(float temperature, float humidity, bool error)
{
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["error"] = error;

    String jsonString;
    serializeJson(doc, jsonString);

    return jsonString;
}