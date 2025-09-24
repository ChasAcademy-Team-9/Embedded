#ifndef SENSORDATAHANDLER_H
#define SENSORDATAHANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "log.h"

struct SensorData
{
    float temperature;
    float humidity;
    bool error;
    ErrorType errorType;
};

float median(std::vector<float>& values);
SensorData calcMedian(JsonArray& arr);

#endif