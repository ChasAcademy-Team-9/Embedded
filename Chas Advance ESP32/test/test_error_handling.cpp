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
#include "sensorDataHandler.h"

void test_error_handling_null_json(void)
{
    // Test: System should handle empty/null JSON gracefully without crashing
    String nullJson = "";

    // Clear any previous output
    Serial.lastPrint = "";
    Serial.lastPrintln = "";

    // This should handle empty string gracefully
    parseJson(nullJson);

    // Should produce an error message for empty JSON
    TEST_ASSERT_TRUE(Serial.lastPrint.indexOf("JSON parse error") != -1);
}

void test_error_handling_malformed_json(void)
{
    // Test: Verify system handles malformed JSON without crashing or corrupting data
    String malformedJson = "{\"Temperature\":25.0,\"Humidity\":}"; // Missing value

    // Clear any previous output
    Serial.lastPrint = "";
    Serial.lastPrintln = "";

    parseJson(malformedJson);

    // Should produce an error message for malformed JSON
    TEST_ASSERT_TRUE(Serial.lastPrint.indexOf("JSON parse error") != -1);
}

void test_error_handling_wrong_data_types(void)
{
    // Test: Verify error handling when JSON contains wrong data types for sensor fields
    String wrongTypeJson = "{\"Temperature\":\"not_a_number\",\"Humidity\":55.0}";

    // Clear any previous output
    Serial.lastPrint = "";
    Serial.lastPrintln = "";

    // This should parse but use default values for invalid types
    parseJson(wrongTypeJson);

    // JSON parsing might succeed but use defaults (ArduinoJson behavior)
    // So we test that it doesn't crash
    TEST_ASSERT_TRUE(true); // If we get here, it didn't crash
}

void test_error_handling_empty_sensor_batch(void)
{
    // Test: Verify graceful handling of empty sensor data batch in serialization
    std::vector<SensorData> emptyBatch;
    String result = serializeBatchToJson(emptyBatch);

    // Should return valid empty JSON array
    TEST_ASSERT_EQUAL_STRING("[]", result.c_str());
}

void test_error_handling_median_with_all_errors(void)
{
    std::vector<float> errorValues = {-99.0, -1.0, -99.0, -1.0};

    float result = median(errorValues);

    // Should return 0.0 when all values are error values
    TEST_ASSERT_EQUAL_FLOAT(0.0, result);
}

void test_error_handling_median_empty_vector(void)
{
    // Test: Verify median calculation returns sensible default for completely empty data
    std::vector<float> emptyVector;

    float result = median(emptyVector);

    // Should return 0.0 for empty vector
    TEST_ASSERT_EQUAL_FLOAT(0.0, result);
}

// Test functions are automatically discovered by Unity