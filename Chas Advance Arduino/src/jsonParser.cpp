#include "jsonParser.h"

// Deprecated: This function is currently unused but retained for legacy support or future use.
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

SensorData getDataFromJson(const String &json) {
    SensorData data;
    
    StaticJsonDocument<200> doc;
    DeserializationError dError = deserializeJson(doc, json);

    if (dError) {
        Serial.print("JSON parse error: ");
        Serial.println(dError.c_str());

        // Return default error value
        data.temperature = 0.0;
        data.humidity = 0.0;
        data.error = true;
        data.errorType = SENSOR_FAIL;
        return data;
    }

    data.temperature = doc["temperature"] | 0.0;
    data.humidity = doc["humidity"] | 0.0;
    data.error = doc["error"] | false;
    int errorTypeInt = doc["errorType"] | 0;
    data.errorType = static_cast<ErrorType>(errorTypeInt);

    return data;
}