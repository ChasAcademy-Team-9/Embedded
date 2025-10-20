/**
 * @file test_main.cpp
 * @brief Main test runner for all ESP32 unit tests
 *
 * This file contains the setup() and loop() functions required by Arduino
 * and runs all the unit tests from different test modules.
 *
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include "mockJson.h"
#include "sensorDataHandler.h"
#include "jsonParser.h"

// Forward declarations for test functions from other files
// Mock JSON tests
void test_mock_json_format(void);
void test_mock_json_valid_ranges(void);
void test_mock_json_error_values(void);

// Sensor data handler tests
void test_median_calculation_odd_values(void);
void test_median_calculation_even_values(void);
void test_median_calculation_empty_vector(void);
void test_calc_median_excludes_errors(void);
void test_calc_median_all_errors(void);

// JSON parser tests
void test_serialize_batch_to_json(void);
void test_serialize_empty_batch(void);

void setup()
{
    delay(2000); // Wait for board to stabilize
    UNITY_BEGIN();

    // Run mock JSON tests
    RUN_TEST(test_mock_json_format);
    RUN_TEST(test_mock_json_valid_ranges);
    RUN_TEST(test_mock_json_error_values);

    // Run sensor data handler tests
    RUN_TEST(test_median_calculation_odd_values);
    RUN_TEST(test_median_calculation_even_values);
    RUN_TEST(test_median_calculation_empty_vector);
    RUN_TEST(test_calc_median_excludes_errors);
    RUN_TEST(test_calc_median_all_errors);

    // Run JSON parser tests
    RUN_TEST(test_serialize_batch_to_json);
    RUN_TEST(test_serialize_empty_batch);

    UNITY_END();
}

void loop()
{
    // Tests run once in setup()
}