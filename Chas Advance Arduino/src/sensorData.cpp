#include "sensorData.h"

 DHT dht(DHTPIN, DHTTYPE);

bool checkThresholds(SensorData &data, const Thresholds &thresholds)
{
    bool exceeded = false;

    if (data.temperature < thresholds.minTemperature || data.temperature > thresholds.maxTemperature)
    {
        exceeded = true;
        data.error = true;
        data.errorType = (data.temperature < thresholds.minTemperature) ? TOO_LOW : TOO_HIGH;
    }
    if (data.humidity < thresholds.minHumidity || data.humidity > thresholds.maxHumidity)
    {
        exceeded = true;
        data.error = true;
        data.errorType = (data.humidity < thresholds.minHumidity) ? TOO_LOW : TOO_HIGH;
    }
    return exceeded;
}

SensorData measureSensorData(uint8_t sensorId, uint32_t timestamp, TemperatureMode currentMode)
{
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    bool error = false;

    // generateMockData(temperature, humidity, error); //For testing without a physical sensor

    SensorData data = {sensorId, timestamp, temperature, humidity, error, NONE};
    if (isnan(humidity) || isnan(temperature))
    {
        data.error = true;
        data.errorType = SENSOR_FAIL;
    }
    if (!data.error)
    {
        checkThresholds(data, getThresholdsForMode(currentMode));
    }

    return data;
}
