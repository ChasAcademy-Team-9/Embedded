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

#include "sensorDataHandler.h"

void test_median_odd_values(void)
{
    // Test: Verify median calculation for odd number of values (middle value)
    std::vector<float> values = {1.0, 3.0, 2.0, 5.0, 4.0};
    float result = median(values);
    TEST_ASSERT_EQUAL_FLOAT(3.0, result);
}

void test_median_even_values(void)
{
    // Test: Verify median calculation for even number of values (average of two middle values)
    std::vector<float> values = {1.0, 2.0, 3.0, 4.0};
    float result = median(values);
    TEST_ASSERT_EQUAL_FLOAT(2.5, result);
}

void test_median_empty_vector(void)
{
    // Test: Verify graceful handling of empty sensor data
    std::vector<float> values;
    float result = median(values);
    TEST_ASSERT_EQUAL_FLOAT(0.0, result);
}

void test_calc_median_excludes_errors(void)
{
    // Test: Verify that error values (-99.0) are excluded from median calculation
    std::vector<float> values = {-99.0, 23.5, 24.0, -99.0, 25.5};
    float result = median(values);
    TEST_ASSERT_EQUAL_FLOAT(24.0, result);
}

// Test functions are automatically discovered by Unity