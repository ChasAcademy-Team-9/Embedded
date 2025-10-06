#include "jsonParser.h"

//not used?
String parseJSON(float temperature, float humidity, bool error)
{
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["error"] = error;
    doc["errorType"] = static_cast<int>(error ? SENSOR_FAIL : NONE);

    String jsonString;
    serializeJson(doc, jsonString);

    return jsonString;
}

String createBatchJson(const std::vector<SensorData> &buffer)
{
    StaticJsonDocument<1024> doc;
    JsonArray arr = doc.to<JsonArray>();
    for (const auto &entry : buffer)
    {
        JsonObject obj = arr.createNestedObject();
        obj["temperature"] = entry.temperature;
        obj["humidity"] = entry.humidity;
        obj["error"] = entry.error;
        obj["errorType"] = static_cast<int>(entry.errorType);
    }
    String output;
    serializeJson(arr, output);
    return output;
}