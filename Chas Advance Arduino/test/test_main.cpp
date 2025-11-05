/**
 * @file test_main.cpp
 * @brief Main test runner for all Arduino unit tests
 *
 * This file contains the setup() and loop() functions required by Arduino
 * and runs all the unit tests from different test modules.
 *
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include "batchHandler.h"
#include "mockSensor.h"
#include "sensorData.h"
#include "thresholds.h"
#include "arduinoLogger.h"

// Global variables required by dependencies
Logger logger;

// Forward declarations for test functions from other files
// Batch handler tests
void test_median_odd_number_of_readings(void);
void test_median_even_number_of_readings(void);
void test_median_excludes_sensor_failures(void);
void test_median_single_reading(void);
void test_median_all_sensor_failures(void);
void test_median_unsorted_data(void);

// Mock sensor tests
void test_mock_data_valid_ranges(void);
void test_mock_data_error_flag_consistency(void);
void test_mock_data_modifies_outputs(void);

// Sensor data tests
void test_valid_sensor_data(void);
void test_temperature_too_low(void);
void test_temperature_too_high(void);
void test_humidity_too_low(void);
void test_humidity_too_high(void);
void test_temperature_at_minimum(void);
void test_temperature_at_maximum(void);

// Thresholds tests
void test_room_temp_thresholds(void);
void test_cooler_thresholds(void);
void test_freezer_thresholds(void);

// Flash functionality tests
void test_get_flash_data_as_batch_empty(void);
void test_get_flash_data_as_batch_with_data(void);
void test_send_flash_data_when_empty(void);

void setup()
{
    delay(2000); // Wait for board to stabilize

    // Initialize the global logger once for all tests
    logger.begin();

    UNITY_BEGIN();

    // Run batch handler tests
    RUN_TEST(test_median_odd_number_of_readings);
    RUN_TEST(test_median_even_number_of_readings);
    RUN_TEST(test_median_excludes_sensor_failures);
    RUN_TEST(test_median_single_reading);
    RUN_TEST(test_median_all_sensor_failures);
    RUN_TEST(test_median_unsorted_data);

    // Run mock sensor tests
    RUN_TEST(test_mock_data_valid_ranges);
    RUN_TEST(test_mock_data_error_flag_consistency);
    RUN_TEST(test_mock_data_modifies_outputs);

    // Run sensor data tests
    RUN_TEST(test_valid_sensor_data);
    RUN_TEST(test_temperature_too_low);
    RUN_TEST(test_temperature_too_high);
    RUN_TEST(test_humidity_too_low);
    RUN_TEST(test_humidity_too_high);
    RUN_TEST(test_temperature_at_minimum);
    RUN_TEST(test_temperature_at_maximum);

    // Run thresholds tests
    RUN_TEST(test_room_temp_thresholds);
    RUN_TEST(test_cooler_thresholds);
    RUN_TEST(test_freezer_thresholds);

    // Run flash functionality tests
    RUN_TEST(test_get_flash_data_as_batch_empty);
    RUN_TEST(test_get_flash_data_as_batch_with_data);
    RUN_TEST(test_send_flash_data_when_empty);

    UNITY_END();
}

void loop()
{
    // Tests run once in setup()
}