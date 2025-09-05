#ifndef MOCKSENSOR_H
#define MOCKSENSOR_H

#include <Arduino.h>

// Funktioner för mockad sensordata
void generateMockData(float &temp, float &hum, bool &error);

#endif
