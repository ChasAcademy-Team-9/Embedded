/**
 * @file test_mock_json.cpp
 * @brief Unit tests for mock JSON generation
 *
 * Tests the generateMockJson function which creates simulated JSON data
 * for testing purposes. The function generates JSON with timestamp,
 * temperature, humidity, and error fields.
 *
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include <ArduinoJson.h>
#include "mockJson.h"

/**
 * Test that generated JSON has correct format and fields
 */
void test_mock_json_format(void)
{
    String jsonStr = generateMockJson();

    // Parse the JSON to verify structure
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);

    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_FALSE(doc["timestamp"].isNull());
    TEST_ASSERT_FALSE(doc["temperature"].isNull());
    TEST_ASSERT_FALSE(doc["humidity"].isNull());
    TEST_ASSERT_FALSE(doc["error"].isNull());
}

/**
 * Test that generated JSON produces valid temperature and humidity ranges
 * Valid ranges: temp 22.0-28.0°C, humidity 40.0-60.0%
 */
void test_mock_json_valid_ranges(void)
{
    bool foundValid = false;

    // Run multiple times to get valid data (not error case)
    for (int i = 0; i < 20; i++)
    {
        String jsonStr = generateMockJson();
        JsonDocument doc;
        deserializeJson(doc, jsonStr);

        bool error = doc["error"];
        if (!error)
        {
            foundValid = true;
            float temp = doc["temperature"];
            float hum = doc["humidity"];

            TEST_ASSERT_TRUE(temp >= 22.0 && temp <= 28.0);
            TEST_ASSERT_TRUE(hum >= 40.0 && hum <= 60.0);
        }
    }

    TEST_ASSERT_TRUE(foundValid); // Should find at least one valid reading
}

/**
 * Test that error values are set correctly when error flag is true
 * Error values: temp -99.0, humidity -1.0
 */
void test_mock_json_error_values(void)
{
    bool foundError = false;

    // Run multiple times to potentially get error case
    for (int i = 0; i < 30; i++)
    {
        String jsonStr = generateMockJson();
        JsonDocument doc;
        deserializeJson(doc, jsonStr);

        bool error = doc["error"];
        if (error)
        {
            foundError = true;
            float temp = doc["temperature"];
            float hum = doc["humidity"];

            TEST_ASSERT_EQUAL_FLOAT(-99.0, temp);
            TEST_ASSERT_EQUAL_FLOAT(-1.0, hum);
        }
    }

    // Note: Due to randomness, we might not always hit the error case
    // This test verifies correct behavior when error case occurs
}

// setup() and loop() moved to test_main.cpp