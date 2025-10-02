#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <Arduino.h>
#include "sensorDataHandler.h"

void parseJson(String json);
void parseJsonArray(JsonArray& arr, const String &timestamp);

// Convert a vector of SensorEntry to a JSON string
String serializeBatchToJson(const std::vector<SensorData> &batch);

#endif