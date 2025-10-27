/**
 * @file test_json_parser.cpp
 * @brief Unit tests for jsonParser functionality in native environment
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

void test_serialize_batch_to_json(void)
{
    // Test: Verify that sensor data batch is correctly serialized to JSON array format
    std::vector<SensorData> batch;

    SensorData data1;
    data1.SensorId = 1;
    data1.temperature = 22.5;
    data1.humidity = 45.0;
    data1.timestamp = 1672574400; // Example Unix timestamp
    data1.error = false;
    batch.push_back(data1);

    String jsonStr = serializeBatchToJson(batch);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr.c_str());

    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_TRUE(doc.is<JsonArray>());

    JsonArray arr = doc.as<JsonArray>();
    TEST_ASSERT_EQUAL(1, arr.size());

    JsonObject obj1 = arr[0];
    TEST_ASSERT_FALSE(obj1["SensorId"].isNull());
    TEST_ASSERT_FALSE(obj1["Temperature"].isNull());
    TEST_ASSERT_FALSE(obj1["Humidity"].isNull());
    TEST_ASSERT_EQUAL_FLOAT(22.5, obj1["Temperature"]);
    TEST_ASSERT_EQUAL_FLOAT(45.0, obj1["Humidity"]);
}

void test_serialize_empty_batch(void)
{
    // Test: Verify graceful handling of empty sensor data batch
    std::vector<SensorData> batch;
    String jsonStr = serializeBatchToJson(batch);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr.c_str());

    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_TRUE(doc.is<JsonArray>());

    JsonArray arr = doc.as<JsonArray>();
    TEST_ASSERT_EQUAL(0, arr.size());
}

void test_parse_json_valid_data(void)
{
    // Test: Verify parsing of valid JSON sensor data with all required fields
    String validJson = "{\"SensorTimeStamp\":\"2024-01-01 12:00:00\",\"Temperature\":23.5,\"Humidity\":55.0,\"error\":false,\"errorType\":0}";

    // Clear any previous output
    Serial.lastPrint = "";
    Serial.lastPrintln = "";

    // This should parse without error
    parseJson(validJson);

    // No error message should be printed
    TEST_ASSERT_TRUE(Serial.lastPrint.isEmpty() || Serial.lastPrint.indexOf("JSON parse error") == -1);
}

void test_parse_json_invalid_data(void)
{
    // Test: Verify proper error handling for malformed JSON data
    String invalidJson = "{invalid json";

    // Clear any previous output
    Serial.lastPrint = "";
    Serial.lastPrintln = "";

    // This should fail to parse and print error message
    parseJson(invalidJson);

    // Error message should be printed
    TEST_ASSERT_TRUE(Serial.lastPrint.indexOf("JSON parse error") != -1);
}

void test_parse_json_missing_fields(void)
{
    // Test: Verify error handling for JSON with missing required sensor fields
    String jsonMissingFields = "{\"Temperature\":20.0}";

    // Clear any previous output
    Serial.lastPrint = "";
    Serial.lastPrintln = "";

    // This should parse but use default values for missing fields
    parseJson(jsonMissingFields);

    // No error message should be printed for missing fields (they get defaults)
    TEST_ASSERT_TRUE(Serial.lastPrint.isEmpty() || Serial.lastPrint.indexOf("JSON parse error") == -1);
}

void test_parse_json_array_functionality(void)
{
    // Test: Verify parsing of JSON array containing multiple sensor data objects
    // Create a JsonDocument for testing
    StaticJsonDocument<512> doc;
    JsonArray arr = doc.to<JsonArray>();

    // Add test objects to the array
    JsonObject obj1 = arr.createNestedObject();
    obj1["Temperature"] = 22.0;
    obj1["Humidity"] = 45.0;
    obj1["error"] = false;

    JsonObject obj2 = arr.createNestedObject();
    obj2["Temperature"] = 24.0;
    obj2["Humidity"] = 55.0;
    obj2["error"] = false;

    String timestamp = "2024-01-01 12:00:00";

    // Clear any previous output
    Serial.lastPrint = "";
    Serial.lastPrintln = "";

    // This should process all objects in the array
    parseJsonArray(arr, timestamp);

    // Should not have any error messages
    TEST_ASSERT_TRUE(Serial.lastPrint.isEmpty() || Serial.lastPrint.indexOf("JSON parse error") == -1);
}

// Test functions are automatically discovered by Unity