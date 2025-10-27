/**
 * @file test_mock_json.cpp
 * @brief Unit tests for mockJson functionality in native environment
 */

#include <unity.h>
#include <ArduinoJson.h>

#ifdef NATIVE_BUILD
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

#include "mockJson.h"

void test_mock_json_format(void)
{
    // Test: Verify that generateMockJson() produces valid JSON with all required fields
    String jsonStr = generateMockJson();

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr.c_str());

    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_FALSE(doc["timestamp"].isNull());
    TEST_ASSERT_FALSE(doc["temperature"].isNull());
    TEST_ASSERT_FALSE(doc["humidity"].isNull());
    TEST_ASSERT_FALSE(doc["error"].isNull());
}

void test_mock_json_temperature_range(void)
{
    // Test: Verify mock returns expected sensor values (important for consistent testing)
    String jsonStr = generateMockJson();
    JsonDocument doc;
    deserializeJson(doc, jsonStr.c_str());

    float temp = doc["temperature"];
    float hum = doc["humidity"];
    bool error = doc["error"];

    // Our mock returns fixed values: temp=25.0, hum=50.0, error=false
    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_EQUAL_FLOAT(25.0, temp);
    TEST_ASSERT_EQUAL_FLOAT(50.0, hum);
}

void test_mock_json_error_values(void)
{
    // Test: Verify error handling behavior in mock data (error=false scenario)
    String jsonStr = generateMockJson();
    JsonDocument doc;
    deserializeJson(doc, jsonStr.c_str());

    bool error = doc["error"];

    // Our mock always returns error=false, so test that
    TEST_ASSERT_FALSE(error);

    // When error is false, temperature and humidity should be normal values
    float temp = doc["temperature"];
    float hum = doc["humidity"];
    TEST_ASSERT_EQUAL_FLOAT(25.0, temp);
    TEST_ASSERT_EQUAL_FLOAT(50.0, hum);
}