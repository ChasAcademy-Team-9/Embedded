#ifndef BATCHHANDLER_H
#define BATCHHANDLER_H

#include "sensorData.h"
#include <vector>
#include <Arduino.h>

void batchSensorReadings(const SensorData &data);
SensorData calculateMedian(std::vector<SensorData>& buffer);
std::vector<SensorData>& getBatchBuffer();
#endif