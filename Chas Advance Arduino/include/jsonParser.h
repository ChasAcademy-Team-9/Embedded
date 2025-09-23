#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <ArduinoJson.h>
#include <vector>
#include "SensorData.h"
#include "log.h"

String parseJSON(float temperature, float humidity, bool error);
String createBatchJson(const std::vector<SensorData> &buffer);

#endif
