#include "sensorDataHandler.h"

float median(std::vector<float>& values) {
    if (values.empty()) return NAN;
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    if (n % 2 == 1) return values[n / 2];
    return (values[n / 2 - 1] + values[n / 2]) / 2.0;
}

SensorData calcMedian(JsonArray& arr) {
    std::vector<float> temps;
    std::vector<float> hums;

    for (JsonObject obj : arr) {
        bool error = obj["error"] | false;
        if (!error) {
            temps.push_back(obj["temperature"] | 0.0);
            hums.push_back(obj["humidity"] | 0.0);
        }
    }

    SensorData medianData;
    medianData.temperature = median(temps);
    medianData.humidity = median(hums);
    medianData.error = temps.empty() || hums.empty();

    return medianData;
}