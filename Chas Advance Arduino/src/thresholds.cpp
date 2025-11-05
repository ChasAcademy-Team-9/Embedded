#include "thresholds.h"

Thresholds roomTempThresholds = { 18.0, 25.0, 30.0, 70.0 };
Thresholds coolerThresholds   = { 2.0, 6.0, 30.0, 70.0 };
Thresholds freezerThresholds  = { -30.0, -18.0, 30.0, 70.0 };

Thresholds getThresholdsForMode(TemperatureMode mode) {
    switch(mode) {
        case ROOM_TEMP: return roomTempThresholds;
        case COOLER:    return coolerThresholds;
        case FREEZER:   return freezerThresholds;
        default:       return roomTempThresholds;
    }
}
