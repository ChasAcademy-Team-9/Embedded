#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "log.h"
#include "thresholds.h"

struct SensorData
{
    uint8_t SensorId;    // ID of the sensor
    uint32_t timestamp;  // milliseconds since Arduino started
    float temperature;
    float humidity;
    bool error;
    uint8_t errorType;
};

bool checkThresholds(SensorData &data, const Thresholds &thresholds);
#endif