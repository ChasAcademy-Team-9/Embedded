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
#include "sensorData.h"

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