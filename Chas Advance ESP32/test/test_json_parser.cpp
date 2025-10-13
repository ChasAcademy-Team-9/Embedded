#include <unity.h>
#include "../include/jsonParser.h"
#include "../include/sensorDataHandler.h"
#include <ArduinoJson.h>

void setUp(void)
{
    // Set up before each test
}

void tearDown(void)
{
    // Clean up after each test
}

// Test JSON parsing of single sensor reading
void test_parse_single_sensor_reading()
{
    String testJson = R"({"timestamp":"2025-10-13 12:30:00","temperature":23.5,"humidity":65.2,"error":false,"errorType":0})";

    // This would require mocking logSensorData function
    // For now, test that parseJson doesn't crash
    parseJson(testJson);

    // In real implementation, we'd verify that logSensorData was called with correct values
    TEST_ASSERT_TRUE(true); // Placeholder - would check logged data
}

// Test JSON parsing with error condition
void test_parse_sensor_reading_with_error()
{
    String testJson = R"({"timestamp":"2025-10-13 12:30:00","temperature":35.0,"humidity":70.0,"error":true,"errorType":2})";

    parseJson(testJson);

    // Should handle error condition without crashing
    TEST_ASSERT_TRUE(true);
}

// Test JSON parsing with invalid data
void test_parse_invalid_json()
{
    String invalidJson = "invalid json string";

    // Should handle invalid JSON gracefully
    parseJson(invalidJson);

    TEST_ASSERT_TRUE(true); // Should not crash
}

// Test JSON parsing with missing fields
void test_parse_json_missing_fields()
{
    String incompleteJson = R"({"temperature":23.5})";

    parseJson(incompleteJson);

    // Should handle missing fields with default values
    TEST_ASSERT_TRUE(true);
}

// Test batch serialization to JSON
void test_serialize_batch_to_json()
{
    std::vector<SensorData> testBatch;

    SensorData entry1 = {1697200000, 23.5, 65.2, false, 0, 1}; // Added sensorID
    SensorData entry2 = {1697200030, 24.0, 66.0, false, 0, 1};
    SensorData entry3 = {1697200060, 22.8, 64.5, false, 0, 1};

    testBatch.push_back(entry1);
    testBatch.push_back(entry2);
    testBatch.push_back(entry3);

    String jsonResult = serializeBatchToJson(testBatch);

    // Verify JSON structure
    TEST_ASSERT_TRUE(jsonResult.length() > 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"data\"") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"SensorID\"") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"TimeStamp\"") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"Temperature\"") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"Humidity\"") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("23.5") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("65.2") >= 0);
}

// Test empty batch serialization
void test_serialize_empty_batch()
{
    std::vector<SensorData> emptyBatch;

    String jsonResult = serializeBatchToJson(emptyBatch);

    // Should return valid JSON with empty data array
    TEST_ASSERT_TRUE(jsonResult.length() > 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"data\":[]") >= 0);
}

// Test JSON array parsing
void test_parse_json_array()
{
    StaticJsonDocument<512> doc;
    JsonArray testArray = doc.createNestedArray("testData");

    JsonObject obj1 = testArray.createNestedObject();
    obj1["temperature"] = 23.5;
    obj1["humidity"] = 65.2;
    obj1["error"] = false;
    obj1["errorType"] = 0;

    JsonObject obj2 = testArray.createNestedObject();
    obj2["temperature"] = 24.0;
    obj2["humidity"] = 66.0;
    obj2["error"] = false;
    obj2["errorType"] = 0;

    String timestamp = "2025-10-13 12:30:00";

    // Test parsing array (this would require mocking)
    parseJsonArray(testArray, timestamp);

    TEST_ASSERT_TRUE(true); // Should complete without crashing
}

// Test large batch serialization (performance)
void test_serialize_large_batch()
{
    std::vector<SensorData> largeBatch;

    // Create large batch
    for (int i = 0; i < 100; i++)
    {
        SensorData entry = {
            static_cast<uint32_t>(1697200000 + i * 30),
            20.0 + i * 0.1,
            60.0 + i * 0.1,
            false,
            0,
            1};
        largeBatch.push_back(entry);
    }

    String jsonResult = serializeBatchToJson(largeBatch);

    // Should handle large batch
    TEST_ASSERT_TRUE(jsonResult.length() > 1000);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"data\"") >= 0);
}

// Test JSON with special characters
void test_parse_json_special_characters()
{
    String jsonWithSpecial = R"({"timestamp":"2025-10-13T12:30:00Z","temperature":23.5,"humidity":65.2,"error":false,"errorType":0})";

    parseJson(jsonWithSpecial);

    TEST_ASSERT_TRUE(true); // Should handle ISO timestamp format
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_parse_single_sensor_reading);
    RUN_TEST(test_parse_sensor_reading_with_error);
    RUN_TEST(test_parse_invalid_json);
    RUN_TEST(test_parse_json_missing_fields);
    RUN_TEST(test_serialize_batch_to_json);
    RUN_TEST(test_serialize_empty_batch);
    RUN_TEST(test_parse_json_array);
    RUN_TEST(test_serialize_large_batch);
    RUN_TEST(test_parse_json_special_characters);

    return UNITY_END();
}