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
    int errorTypeInt = doc["errorType"] | 0; // safe fallback
    ErrorType errorType = static_cast<ErrorType>(errorTypeInt);

    logSensorData(timestamp, temperature, humidity, errorType);
}

void parseJsonArray(JsonArray& arr, const String &timestamp)
{
    for (JsonObject obj : arr)
    {
        obj["timestamp"] = timestamp;
        String updatedBody;
        serializeJson(obj, updatedBody);
        parseJson(updatedBody);
    }
}

String serializeBatchToJson(const std::vector<SensorData> &batch)
{
    StaticJsonDocument<2048> doc;
    JsonArray dataArr = doc.createNestedArray("data");

    for (const auto &entry : batch)
    {
        JsonObject obj = dataArr.createNestedObject();
        obj["SensorID"] = entry.SensorId; // Hardcoded for now
        obj["timestamp"] = formatUnixTime(entry.timestamp); 
        obj["temperature"] = entry.temperature;
        obj["humidity"] = entry.humidity;
        //obj["error"] = entry.error;
        //obj["errorType"] = entry.errorType;
    }

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}
