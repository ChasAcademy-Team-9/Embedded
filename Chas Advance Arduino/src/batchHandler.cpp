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
        Serial.println("Batch interval reached, preparing to send batch");
        return true; // Start to attempt sending the batch
    }
    return false;
}

SensorData calculateMedian(std::vector<SensorData> &buffer)
{
    std::vector<uint32_t> timestamps;
    std::vector<float> temps;
    std::vector<float> hums;

    for (const auto &data : buffer)
    {
        if (data.errorType != ErrorType::SENSOR_FAIL)
        {
            timestamps.push_back(data.timestamp);
            temps.push_back(data.temperature);
            hums.push_back(data.humidity);
        }
    }

    auto medianFloat = [](std::vector<float> &vec) -> float
    {
        if (vec.empty())
            return NAN;
        std::sort(vec.begin(), vec.end());
        size_t mid = vec.size() / 2;
        return (vec.size() % 2 != 0) ? vec[mid] : (vec[mid - 1] + vec[mid]) / 2.0;
    };

    auto medianUint32 = [](std::vector<uint32_t> &vec) -> uint32_t
    {
        if (vec.empty())
            return 0;
        std::sort(vec.begin(), vec.end());
        size_t mid = vec.size() / 2;
        return (vec.size() % 2 != 0) ? vec[mid]
                                     : static_cast<uint32_t>((vec[mid - 1] + vec[mid]) / 2);
    };

    SensorData medianData;
    medianData.SensorId = buffer.empty() ? 1 : buffer.front().SensorId;
    medianData.timestamp = medianUint32(timestamps);
    medianData.temperature = medianFloat(temps);
    medianData.humidity = medianFloat(hums);
    medianData.error = temps.empty() || hums.empty();
    medianData.errorType = medianData.error ? ErrorType::SENSOR_FAIL : ErrorType::NONE;

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
