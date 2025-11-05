#ifndef THRESHOLDS_H
#define THRESHOLDS_H

enum TemperatureMode {
    ROOM_TEMP,
    COOLER,
    FREEZER
};

struct Thresholds {
    float minTemperature;
    float maxTemperature;
    float minHumidity;
    float maxHumidity;
};

Thresholds getThresholdsForMode(TemperatureMode mode);


#endif
