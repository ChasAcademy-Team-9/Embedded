/**
 * @file test_json_parser.cpp
 * @brief Unit tests for JSON parsing and serialization functions
 *
 * Tests the serializeBatchToJson function which converts a vector
 * of SensorData to JSON format.
 *
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include <ArduinoJson.h>
#include "jsonParser.h"
#include "sensorDataHandler.h"

/**
 * Test serialization of sensor data batch to JSON
 */
void test_serialize_batch_to_json(void)
{
    std::vector<SensorData> batch;

    // Create test data
    SensorData data1 = {1, 1234567890, 22.5, 45.0, false, 0};
    SensorData data2 = {1, 1234567891, 23.0, 50.0, false, 0};
    batch.push_back(data1);
    batch.push_back(data2);

    String jsonStr = serializeBatchToJson(batch);

    // Parse the result to verify structure
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);

    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_TRUE(doc.is<JsonArray>());

    JsonArray arr = doc.as<JsonArray>();
    TEST_ASSERT_EQUAL(2, arr.size());

    // Check first entry
    JsonObject obj1 = arr[0];
    TEST_ASSERT_FALSE(obj1["ArduinoID"].isNull());
    TEST_ASSERT_FALSE(obj1["Temperature"].isNull());
    TEST_ASSERT_FALSE(obj1["Humidity"].isNull());
    TEST_ASSERT_EQUAL_FLOAT(22.5, obj1["Temperature"]);
    TEST_ASSERT_EQUAL_FLOAT(45.0, obj1["Humidity"]);
}

/**
 * Test serialization of empty batch
 */
void test_serialize_empty_batch(void)
{
    std::vector<SensorData> batch; // Empty batch

    String jsonStr = serializeBatchToJson(batch);

    // Parse the result to verify it's an empty array
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);

    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_TRUE(doc.is<JsonArray>());

    JsonArray arr = doc.as<JsonArray>();
    TEST_ASSERT_EQUAL(0, arr.size());
}

// setup() and loop() moved to test_main.cpp