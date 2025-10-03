#include "sensorData.h"

bool checkThresholds(SensorData &data, const Thresholds &thresholds)
{
    bool exceeded = false;

    if(data.temperature < thresholds.minTemperature || data.temperature > thresholds.maxTemperature) {
        exceeded = true;
        data.error = true; 
        data.errorType = (data.temperature < thresholds.minTemperature) ? TOO_LOW : TOO_HIGH;
    }
    if(data.humidity < thresholds.minHumidity || data.humidity > thresholds.maxHumidity) {
        exceeded = true;
        data.error = true;
        data.errorType = (data.humidity < thresholds.minHumidity) ? TOO_LOW : TOO_HIGH;
    }
    return exceeded;
}
