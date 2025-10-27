#ifndef MOCKSJSON_H
#define MOCKSJSON_H

#ifdef NATIVE_BUILD
#include "Arduino.h" // Mock Arduino for native builds
#else
#include <Arduino.h> // Real Arduino for ESP32 builds
#endif

// Funktioner för mockad sensordata
String generateMockJson();

#endif
