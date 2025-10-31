/**
 * @file test_error_handling.cpp
 * @brief Unit tests for error handlivoid test_error_handling_median_with_all_errors(void)
{
    // Test: Verify median calculation when all sensor values are error values (-99.0)
    std::vector<float> errorValues = {-99.0, -99.0, -99.0, -99.0};scenarios
 */

#include <unity.h>
#include <ArduinoJson.h>
#include <vector>

#ifdef NATIVE_BUILD
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

#include "jsonParser.h"
#include "sensorData.h"

void test_error_handling_empty_sensor_batch(void)
{
    // Test: Verify graceful handling of empty sensor data batch in serialization
    std::vector<SensorData> emptyBatch;
    String result = serializeBatchToJson(emptyBatch);

    // Should return valid empty JSON array
    TEST_ASSERT_EQUAL_STRING("[]", result.c_str());
}

// Test functions are automatically discovered by Unity