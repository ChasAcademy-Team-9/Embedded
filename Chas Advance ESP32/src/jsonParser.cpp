#include <ArduinoJson.h>
#include "jsonParser.h"


String serializeBatchToJson(const std::vector<SensorData> &batch)
{
    // Use a sufficiently large static document; adjust size if your batches grow
    StaticJsonDocument<2048> doc;
    JsonArray dataArr = doc.to<JsonArray>();

    for (const auto &entry : batch)
    {
        JsonObject obj = dataArr.createNestedObject();
        obj["ArduinoID"] = entry.SensorId; // Hardcoded for now
        obj["SensorTimeStamp"] = formatUnixTime(entry.timestamp);
        obj["Temperature"] = entry.temperature;
        obj["Humidity"] = entry.humidity;
    }

    String jsonString;
    serializeJson(doc, jsonString); // serialize the array to string
    return jsonString;
}
