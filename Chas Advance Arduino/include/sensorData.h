#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "log.h"
#include "thresholds.h"

#pragma pack(push, 1)
struct SensorData
{
    uint8_t SensorId;    /**< ID of the sensor */
    uint32_t timestamp;   /**< Unix timestamp (seconds since epoch) from ESP32, or 0 if not yet synced */
    float temperature;    /**< Temperature value */
    float humidity;       /**< Humidity value */
    bool error;           /**< Whether there was an error in this reading */
    uint8_t errorType;    /**< Type of error, if any */
};
#pragma pack(pop)
bool checkThresholds(SensorData &data, const Thresholds &thresholds);
#endif