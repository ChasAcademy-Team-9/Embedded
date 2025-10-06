#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "log.h"
#include "thresholds.h"

struct SensorData
{
    float temperature;
    float humidity;
    bool error;
    ErrorType errorType;
};

bool checkThresholds(SensorData &data, const Thresholds &thresholds);
#endif