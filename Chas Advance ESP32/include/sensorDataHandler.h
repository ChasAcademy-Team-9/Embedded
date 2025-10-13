#ifndef SENSORDATAHANDLER_H
#define SENSORDATAHANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "log.h"

struct SensorData
{
    uint8_t SensorId;    /**< ID of the sensor */
    uint32_t timestamp;   /**< Unix timestamp of the sensor reading */
    float temperature;    /**< Temperature value */
    float humidity;       /**< Humidity value */
    bool error;           /**< Whether there was an error in this reading */
    uint8_t errorType;    /**< Type of error, if any */
};

float median(std::vector<float>& values);
SensorData calcMedian(JsonArray& arr);

#endif