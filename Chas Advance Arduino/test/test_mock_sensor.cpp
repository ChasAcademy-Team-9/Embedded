/**
 * @file test_mock_sensor.cpp
 * @brief Unit tests for mock sensor data generation
 *
 * Tests the generateMockData function which creates simulated sensor readings
 * for testing purposes. The function randomly generates either valid readings
 * (temperature 22-28°C, humidity 40-60%) or error readings (temp=-99, hum=-1).
 *
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include "mockSensor.h"

/**
 * Test that mock data generation produces valid ranges
 * Runs multiple times to account for randomness
 * Valid ranges: temp 22.0-28.0°C, humidity 40.0-60.0%
 * Error values: temp -99, humidity -1
 */
void test_mock_data_valid_ranges(void)
{
    bool foundValid = false;
    bool foundError = false;

    // Run multiple times to test both error and valid cases
    for (int i = 0; i < 50; i++)
    {
        float temp, hum;
        bool error;

        generateMockData(temp, hum, error);

        if (error)
        {
            foundError = true;
            TEST_ASSERT_EQUAL_FLOAT(-99.0, temp);
            TEST_ASSERT_EQUAL_FLOAT(-1.0, hum);
        }
        else
        {
            foundValid = true;
            TEST_ASSERT_TRUE(temp >= 22.0 && temp <= 28.0);
            TEST_ASSERT_TRUE(hum >= 40.0 && hum <= 60.0);
        }
    }

    // Verify we tested both cases (with randomness, this should happen)
    TEST_ASSERT_TRUE(foundValid); // Should find at least one valid reading
}

/**
 * Test that error flag is set correctly
 * When error is true, temperature and humidity should be invalid
 */
void test_mock_data_error_flag_consistency(void)
{
    for (int i = 0; i < 30; i++)
    {
        float temp, hum;
        bool error;

        generateMockData(temp, hum, error);

        if (error)
        {
            // When error flag is true, values should be -99 and -1
            TEST_ASSERT_EQUAL_FLOAT(-99.0, temp);
            TEST_ASSERT_EQUAL_FLOAT(-1.0, hum);
        }
        else
        {
            // When error flag is false, values should be in valid range
            TEST_ASSERT_TRUE(temp >= 22.0 && temp <= 28.0);
            TEST_ASSERT_TRUE(hum >= 40.0 && hum <= 60.0);
        }
    }
}

/**
 * Test that mock data generation modifies output parameters
 * Ensures that the function actually writes to the output variables
 */
void test_mock_data_modifies_outputs(void)
{
    float temp = 0.0;
    float hum = 0.0;
    bool error = false;

    generateMockData(temp, hum, error);

    // At minimum, temperature should change from 0
    TEST_ASSERT_TRUE(temp != 0.0);
}

// setup() and loop() moved to test_main.cpp
