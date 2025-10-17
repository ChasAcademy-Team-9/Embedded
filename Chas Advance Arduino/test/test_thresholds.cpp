/**
 * @file test_thresholds.cpp
 * @brief Unit tests for threshold functions
 * 
 * Tests the getThresholdsForMode function which returns different
 * temperature and humidity thresholds based on the operating mode
 * (ROOM_TEMP, COOLER, FREEZER).
 * 
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include "thresholds.h"

/**
 * Test that ROOM_TEMP mode returns correct thresholds
 * Expected: min temp 18.0, max temp 25.0, min hum 30.0, max hum 70.0
 */
void test_room_temp_thresholds(void) {
    Thresholds result = getThresholdsForMode(ROOM_TEMP);
    TEST_ASSERT_EQUAL_FLOAT(18.0, result.minTemperature);
    TEST_ASSERT_EQUAL_FLOAT(25.0, result.maxTemperature);
    TEST_ASSERT_EQUAL_FLOAT(30.0, result.minHumidity);
    TEST_ASSERT_EQUAL_FLOAT(70.0, result.maxHumidity);
}

/**
 * Test that COOLER mode returns correct thresholds
 * Expected: min temp 2.0, max temp 6.0, min hum 30.0, max hum 70.0
 */
void test_cooler_thresholds(void) {
    Thresholds result = getThresholdsForMode(COOLER);
    TEST_ASSERT_EQUAL_FLOAT(2.0, result.minTemperature);
    TEST_ASSERT_EQUAL_FLOAT(6.0, result.maxTemperature);
    TEST_ASSERT_EQUAL_FLOAT(30.0, result.minHumidity);
    TEST_ASSERT_EQUAL_FLOAT(70.0, result.maxHumidity);
}

/**
 * Test that FREEZER mode returns correct thresholds
 * Expected: min temp -30.0, max temp -18.0, min hum 30.0, max hum 70.0
 */
void test_freezer_thresholds(void) {
    Thresholds result = getThresholdsForMode(FREEZER);
    TEST_ASSERT_EQUAL_FLOAT(-30.0, result.minTemperature);
    TEST_ASSERT_EQUAL_FLOAT(-18.0, result.maxTemperature);
    TEST_ASSERT_EQUAL_FLOAT(30.0, result.minHumidity);
    TEST_ASSERT_EQUAL_FLOAT(70.0, result.maxHumidity);
}

void setup() {
    delay(2000); // Wait for board to stabilize
    UNITY_BEGIN();
    RUN_TEST(test_room_temp_thresholds);
    RUN_TEST(test_cooler_thresholds);
    RUN_TEST(test_freezer_thresholds);
    UNITY_END();
}

void loop() {
    // Tests run once in setup()
}
