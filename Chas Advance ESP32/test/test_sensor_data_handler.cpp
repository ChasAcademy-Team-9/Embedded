#include <unity.h>
#include "../include/sensorDataHandler.h"
#include <ArduinoJson.h>
#include <vector>

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

// Test median calculation with odd number of values
void test_median_odd_values() {
    std::vector<float> values = {1.0, 3.0, 2.0, 5.0, 4.0}; // Should be sorted to: 1,2,3,4,5
    
    float result = median(values);
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.0, result); // Median of 5 values is middle (3rd) value
}

// Test median calculation with even number of values
void test_median_even_values() {
    std::vector<float> values = {1.0, 2.0, 3.0, 4.0}; // Median should be (2+3)/2 = 2.5
    
    float result = median(values);
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.5, result);
}

// Test median calculation with single value
void test_median_single_value() {
    std::vector<float> values = {42.0};
    
    float result = median(values);
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, 42.0, result);
}

// Test median calculation with empty vector
void test_median_empty_values() {
    std::vector<float> values;
    
    float result = median(values);
    
    TEST_ASSERT_TRUE(isnan(result)); // Should return NAN for empty vector
}

// Test median calculation with duplicate values
void test_median_duplicate_values() {
    std::vector<float> values = {2.0, 2.0, 2.0, 2.0, 2.0};
    
    float result = median(values);
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, result);
}

// Test median calculation with negative values
void test_median_negative_values() {
    std::vector<float> values = {-5.0, -2.0, -8.0, -1.0, -3.0}; // Sorted: -8,-5,-3,-2,-1
    
    float result = median(values);
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, -3.0, result); // Middle value
}

// Test calcMedian with normal sensor data
void test_calc_median_normal_data() {
    StaticJsonDocument<512> doc;
    JsonArray arr = doc.createNestedArray("data");
    
    // Add normal sensor readings
    JsonObject obj1 = arr.createNestedObject();
    obj1["temperature"] = 20.0;
    obj1["humidity"] = 60.0;
    obj1["error"] = false;
    
    JsonObject obj2 = arr.createNestedObject();
    obj2["temperature"] = 22.0;
    obj2["humidity"] = 62.0;
    obj2["error"] = false;
    
    JsonObject obj3 = arr.createNestedObject();
    obj3["temperature"] = 24.0;
    obj3["humidity"] = 64.0;
    obj3["error"] = false;
    
    SensorData result = calcMedian(arr);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1, 22.0, result.temperature); // Median of 20,22,24
    TEST_ASSERT_FLOAT_WITHIN(0.1, 62.0, result.humidity);   // Median of 60,62,64
    TEST_ASSERT_FALSE(result.error);
}

// Test calcMedian with some error readings
void test_calc_median_with_errors() {
    StaticJsonDocument<512> doc;
    JsonArray arr = doc.createNestedArray("data");
    
    // Add normal reading
    JsonObject obj1 = arr.createNestedObject();
    obj1["temperature"] = 20.0;
    obj1["humidity"] = 60.0;
    obj1["error"] = false;
    
    // Add error reading (should be excluded)
    JsonObject obj2 = arr.createNestedObject();
    obj2["temperature"] = 999.0; // Error value
    obj2["humidity"] = 999.0;    // Error value
    obj2["error"] = true;
    
    // Add another normal reading
    JsonObject obj3 = arr.createNestedObject();
    obj3["temperature"] = 24.0;
    obj3["humidity"] = 64.0;
    obj3["error"] = false;
    
    SensorData result = calcMedian(arr);
    
    // Should calculate median from non-error readings only
    TEST_ASSERT_FLOAT_WITHIN(0.1, 22.0, result.temperature); // (20+24)/2
    TEST_ASSERT_FLOAT_WITHIN(0.1, 62.0, result.humidity);   // (60+64)/2
    TEST_ASSERT_FALSE(result.error);
}

// Test calcMedian with all error readings
void test_calc_median_all_errors() {
    StaticJsonDocument<512> doc;
    JsonArray arr = doc.createNestedArray("data");
    
    // Add only error readings
    JsonObject obj1 = arr.createNestedObject();
    obj1["temperature"] = 999.0;
    obj1["humidity"] = 999.0;
    obj1["error"] = true;
    
    JsonObject obj2 = arr.createNestedObject();
    obj2["temperature"] = 888.0;
    obj2["humidity"] = 888.0;
    obj2["error"] = true;
    
    SensorData result = calcMedian(arr);
    
    // Should set error flag when no valid data
    TEST_ASSERT_TRUE(result.error);
}

// Test calcMedian with empty array
void test_calc_median_empty_array() {
    StaticJsonDocument<512> doc;
    JsonArray arr = doc.createNestedArray("data");
    
    SensorData result = calcMedian(arr);
    
    // Should handle empty array gracefully
    TEST_ASSERT_TRUE(result.error);
}

// Test calcMedian with missing temperature field
void test_calc_median_missing_fields() {
    StaticJsonDocument<512> doc;
    JsonArray arr = doc.createNestedArray("data");
    
    JsonObject obj1 = arr.createNestedObject();
    // Missing temperature field - should default to 0.0
    obj1["humidity"] = 60.0;
    obj1["error"] = false;
    
    JsonObject obj2 = arr.createNestedObject();
    obj2["temperature"] = 22.0;
    // Missing humidity field - should default to 0.0
    obj2["error"] = false;
    
    SensorData result = calcMedian(arr);
    
    // Should handle missing fields with defaults
    TEST_ASSERT_FLOAT_WITHIN(0.1, 11.0, result.temperature); // (0+22)/2
    TEST_ASSERT_FLOAT_WITHIN(0.1, 30.0, result.humidity);   // (60+0)/2
}

// Test median with very large dataset
void test_median_large_dataset() {
    std::vector<float> largeValues;
    
    // Create large dataset
    for (int i = 0; i < 1000; i++) {
        largeValues.push_back(i * 0.1);
    }
    
    float result = median(largeValues);
    
    // Median of 0, 0.1, 0.2, ... 99.9 should be around 49.95
    TEST_ASSERT_FLOAT_WITHIN(0.1, 49.95, result);
}

// Test median with floating point precision
void test_median_float_precision() {
    std::vector<float> values = {1.111, 1.112, 1.113};
    
    float result = median(values);
    
    TEST_ASSERT_FLOAT_WITHIN(0.001, 1.112, result);
}

// Test SensorData structure initialization
void test_sensor_data_structure() {
    SensorData data;
    data.timestamp = 1697200000;
    data.temperature = 23.5;
    data.humidity = 65.2;
    data.error = false;
    data.errorType = 0;
    data.sensorID = 1;
    
    TEST_ASSERT_EQUAL_UINT32(1697200000, data.timestamp);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.5, data.temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 65.2, data.humidity);
    TEST_ASSERT_FALSE(data.error);
    TEST_ASSERT_EQUAL_INT(0, data.errorType);
    TEST_ASSERT_EQUAL_INT(1, data.sensorID);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_median_odd_values);
    RUN_TEST(test_median_even_values);
    RUN_TEST(test_median_single_value);
    RUN_TEST(test_median_empty_values);
    RUN_TEST(test_median_duplicate_values);
    RUN_TEST(test_median_negative_values);
    RUN_TEST(test_calc_median_normal_data);
    RUN_TEST(test_calc_median_with_errors);
    RUN_TEST(test_calc_median_all_errors);
    RUN_TEST(test_calc_median_empty_array);
    RUN_TEST(test_calc_median_missing_fields);
    RUN_TEST(test_median_large_dataset);
    RUN_TEST(test_median_float_precision);
    RUN_TEST(test_sensor_data_structure);
    
    return UNITY_END();
}