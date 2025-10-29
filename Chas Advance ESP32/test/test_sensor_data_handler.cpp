/**
 * @file test_sensor_data_handler.cpp
 * @brief Unit tests for sensorDataHandler functionality in native environment
 */

#include <unity.h>
#include <ArduinoJson.h>
#include <vector>

#ifdef NATIVE_BUILD
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

#include "sensorData.h"


// Test functions are automatically discovered by Unity