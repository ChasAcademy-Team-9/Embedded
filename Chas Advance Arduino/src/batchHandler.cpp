#include "batchHandler.h"
#include "arduinoLogger.h"
#include "wifiHandler.h"

static std::vector<SensorData> batchBuffer;
unsigned long batchSendInterval = 30000;
unsigned long batchStartTime = 0;

extern Logger logger;
extern TemperatureMode currentMode;

bool batchSensorReadings(const SensorData &data)
{
    batchBuffer.push_back(data);
    if (batchStartTime == 0)
        batchStartTime = millis();

    if (millis() - batchStartTime >= batchSendInterval)
    {
#ifdef DEBUG
        Serial.println(millis() - batchStartTime);
#endif
        Serial.println("Batch interval reached, preparing to send batch");
        return true; // Start to attempt sending the batch
    }
    return false;
}

SensorData calculateMedian(std::vector<SensorData> &buffer)
{
    std::vector<float> temps;
    std::vector<float> hums;

    for (const auto &data : buffer)
    {
        if (data.errorType != ErrorType::SENSOR_FAIL)
        {
            temps.push_back(data.temperature);
            hums.push_back(data.humidity);
        }
    }

    auto median = [](std::vector<float> &vec) -> float
    {
        if (vec.empty())
            return NAN;
        std::sort(vec.begin(), vec.end());
        size_t mid = vec.size() / 2;
        return (vec.size() % 2 != 0) ? vec[mid] : (vec[mid - 1] + vec[mid]) / 2.0;
    };

    SensorData medianData;
    medianData.temperature = median(temps);
    medianData.humidity = median(hums);
    medianData.error = temps.empty() || hums.empty();

    return medianData;
}

std::vector<SensorData> &getBatchBuffer()
{
    return batchBuffer;
}

void resetBatchTimer()
{
    batchStartTime = millis();
}
