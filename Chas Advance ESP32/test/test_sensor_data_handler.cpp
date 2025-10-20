/**
 * @file test_sensor_data_handler.cpp
 * @brief Unit tests for sensor data handling functions
 *
 * Tests the median calculation functions and JSON array processing
 * for calculating median values from sensor readings.
 *
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include <ArduinoJson.h>
#include "sensorDataHandler.h"
#include <vector>

/**
 * Test median calculation with odd number of values
 */
void test_median_calculation_odd_values(void)
{
    std::vector<float> values = {20.0, 22.0, 24.0};
    float result = median(values);
    TEST_ASSERT_EQUAL_FLOAT(22.0, result);
}

/**
 * Test median calculation with even number of values
 */
void test_median_calculation_even_values(void)
{
    std::vector<float> values = {20.0, 22.0, 24.0, 26.0};
    float result = median(values);
    TEST_ASSERT_EQUAL_FLOAT(23.0, result); // (22.0 + 24.0) / 2
}

/**
 * Test median calculation with empty vector
 */
void test_median_calculation_empty_vector(void)
{
    std::vector<float> values;
    float result = median(values);
    TEST_ASSERT_TRUE(isnan(result));
}

/**
 * Test calcMedian function excludes error readings
 */
void test_calc_median_excludes_errors(void)
{
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    // Add valid readings
    JsonObject obj1 = arr.add<JsonObject>();
    obj1["temperature"] = 20.0;
    obj1["humidity"] = 40.0;
    obj1["error"] = false;

    JsonObject obj2 = arr.add<JsonObject>();
    obj2["temperature"] = 24.0;
    obj2["humidity"] = 60.0;
    obj2["error"] = false;

    // Add error reading (should be excluded)
    JsonObject obj3 = arr.add<JsonObject>();
    obj3["temperature"] = -99.0;
    obj3["humidity"] = -1.0;
    obj3["error"] = true;

    SensorData result = calcMedian(arr);

    TEST_ASSERT_EQUAL_FLOAT(22.0, result.temperature); // median of 20.0, 24.0
    TEST_ASSERT_EQUAL_FLOAT(50.0, result.humidity);    // median of 40.0, 60.0
    TEST_ASSERT_FALSE(result.error);
}

/**
 * Test calcMedian function with all error readings
 */
void test_calc_median_all_errors(void)
{
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    // Add only error readings
    JsonObject obj1 = arr.add<JsonObject>();
    obj1["temperature"] = -99.0;
    obj1["humidity"] = -1.0;
    obj1["error"] = true;

    JsonObject obj2 = arr.add<JsonObject>();
    obj2["temperature"] = -99.0;
    obj2["humidity"] = -1.0;
    obj2["error"] = true;

    SensorData result = calcMedian(arr);

    TEST_ASSERT_TRUE(isnan(result.temperature));
    TEST_ASSERT_TRUE(isnan(result.humidity));
    TEST_ASSERT_TRUE(result.error);
}

// setup() and loop() moved to test_main.cpp