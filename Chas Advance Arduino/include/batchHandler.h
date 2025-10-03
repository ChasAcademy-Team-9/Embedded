#ifndef BATCHHANDLER_H
#define BATCHHANDLER_H

#include "sensorData.h"
#include <vector>
#include <Arduino.h>

struct FailedBatch {
    std::vector<SensorData> batch;
    uint8_t retries;
};

void batchSensorReadings(const SensorData &data);
SensorData calculateMedian(std::vector<SensorData>& buffer);
std::vector<SensorData>& getBatchBuffer();
#endif