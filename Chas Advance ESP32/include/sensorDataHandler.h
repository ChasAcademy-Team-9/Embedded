#ifndef SENSORDATAHANDLER_H
#define SENSORDATAHANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

struct SensorData
{
    float temperature;
    float humidity;
    bool error;
};

float median(std::vector<float>& values);
SensorData calcMedian(JsonArray& arr);

#endif