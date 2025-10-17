/**
 * @file test_batch_handler.cpp
 * @brief Unit tests for batch handling and median calculation
 * 
 * Tests the calculateMedian function which processes a vector of sensor
 * readings and calculates the median temperature and humidity values,
 * excluding any readings with SENSOR_FAIL errors.
 * 
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include "batchHandler.h"
#include "sensorData.h"
#include <vector>

/**
 * Test median calculation with odd number of valid readings
 * Input: 3 readings with temps [20.0, 22.0, 24.0] and hums [40.0, 50.0, 60.0]
 * Expected median: temp 22.0, humidity 50.0
 */
void test_median_odd_number_of_readings(void) {
    std::vector<SensorData> buffer;
    buffer.push_back({1, 0, 20.0, 40.0, false, NONE});
    buffer.push_back({1, 0, 22.0, 50.0, false, NONE});
    buffer.push_back({1, 0, 24.0, 60.0, false, NONE});
    
    SensorData result = calculateMedian(buffer);
    
    TEST_ASSERT_EQUAL_FLOAT(22.0, result.temperature);
    TEST_ASSERT_EQUAL_FLOAT(50.0, result.humidity);
    TEST_ASSERT_FALSE(result.error);
}

/**
 * Test median calculation with even number of valid readings
 * Input: 4 readings with temps [20.0, 22.0, 24.0, 26.0] and hums [40.0, 50.0, 60.0, 70.0]
 * Expected median: temp 23.0 (avg of 22.0 and 24.0), humidity 55.0 (avg of 50.0 and 60.0)
 */
void test_median_even_number_of_readings(void) {
    std::vector<SensorData> buffer;
    buffer.push_back({1, 0, 20.0, 40.0, false, NONE});
    buffer.push_back({1, 0, 22.0, 50.0, false, NONE});
    buffer.push_back({1, 0, 24.0, 60.0, false, NONE});
    buffer.push_back({1, 0, 26.0, 70.0, false, NONE});
    
    SensorData result = calculateMedian(buffer);
    
    TEST_ASSERT_EQUAL_FLOAT(23.0, result.temperature);
    TEST_ASSERT_EQUAL_FLOAT(55.0, result.humidity);
    TEST_ASSERT_FALSE(result.error);
}

/**
 * Test median calculation excludes SENSOR_FAIL readings
 * Input: 5 readings where one has SENSOR_FAIL
 * Expected: median calculated from 4 valid readings only
 */
void test_median_excludes_sensor_failures(void) {
    std::vector<SensorData> buffer;
    buffer.push_back({1, 0, 20.0, 40.0, false, NONE});
    buffer.push_back({1, 0, 22.0, 50.0, false, NONE});
    buffer.push_back({1, 0, -99.0, -1.0, true, SENSOR_FAIL}); // Should be excluded
    buffer.push_back({1, 0, 24.0, 60.0, false, NONE});
    buffer.push_back({1, 0, 26.0, 70.0, false, NONE});
    
    SensorData result = calculateMedian(buffer);
    
    TEST_ASSERT_EQUAL_FLOAT(23.0, result.temperature);
    TEST_ASSERT_EQUAL_FLOAT(55.0, result.humidity);
    TEST_ASSERT_FALSE(result.error);
}

/**
 * Test median calculation with single reading
 * Input: 1 reading with temp 22.0 and humidity 50.0
 * Expected median: same values as input
 */
void test_median_single_reading(void) {
    std::vector<SensorData> buffer;
    buffer.push_back({1, 0, 22.0, 50.0, false, NONE});
    
    SensorData result = calculateMedian(buffer);
    
    TEST_ASSERT_EQUAL_FLOAT(22.0, result.temperature);
    TEST_ASSERT_EQUAL_FLOAT(50.0, result.humidity);
    TEST_ASSERT_FALSE(result.error);
}

/**
 * Test median calculation with all SENSOR_FAIL readings
 * Input: all readings have SENSOR_FAIL error
 * Expected: error flag should be true, values should be NAN
 */
void test_median_all_sensor_failures(void) {
    std::vector<SensorData> buffer;
    buffer.push_back({1, 0, -99.0, -1.0, true, SENSOR_FAIL});
    buffer.push_back({1, 0, -99.0, -1.0, true, SENSOR_FAIL});
    buffer.push_back({1, 0, -99.0, -1.0, true, SENSOR_FAIL});
    
    SensorData result = calculateMedian(buffer);
    
    TEST_ASSERT_TRUE(result.error);
    TEST_ASSERT_TRUE(isnan(result.temperature));
    TEST_ASSERT_TRUE(isnan(result.humidity));
}

/**
 * Test median calculation with unsorted data
 * Input: readings in random order
 * Expected: correct median regardless of input order
 */
void test_median_unsorted_data(void) {
    std::vector<SensorData> buffer;
    buffer.push_back({1, 0, 26.0, 70.0, false, NONE});
    buffer.push_back({1, 0, 20.0, 40.0, false, NONE});
    buffer.push_back({1, 0, 24.0, 60.0, false, NONE});
    buffer.push_back({1, 0, 22.0, 50.0, false, NONE});
    
    SensorData result = calculateMedian(buffer);
    
    // Median should be (22.0 + 24.0) / 2 = 23.0 for temp
    // and (50.0 + 60.0) / 2 = 55.0 for humidity
    TEST_ASSERT_EQUAL_FLOAT(23.0, result.temperature);
    TEST_ASSERT_EQUAL_FLOAT(55.0, result.humidity);
    TEST_ASSERT_FALSE(result.error);
}

void setup() {
    delay(2000); // Wait for board to stabilize
    UNITY_BEGIN();
    RUN_TEST(test_median_odd_number_of_readings);
    RUN_TEST(test_median_even_number_of_readings);
    RUN_TEST(test_median_excludes_sensor_failures);
    RUN_TEST(test_median_single_reading);
    RUN_TEST(test_median_all_sensor_failures);
    RUN_TEST(test_median_unsorted_data);
    UNITY_END();
}

void loop() {
    // Tests run once in setup()
}
