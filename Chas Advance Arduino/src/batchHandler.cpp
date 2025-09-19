#include "batchHandler.h"
#include <vector>
#include "jsonParser.h"
#include "wifiHandler.h"

static std::vector<SensorData> batchBuffer;
static unsigned long batchStartTime = 0;

void batchSensorReadings(const SensorData &data)
{
    batchBuffer.push_back(data);

    if (batchStartTime == 0)
        batchStartTime = millis();

    if (millis() - batchStartTime >= 30000)
    {
        String batchJson = createBatchJson(batchBuffer);
        sendDataToESP32(batchJson);
        batchBuffer.clear();
        batchStartTime = millis();
    }
}