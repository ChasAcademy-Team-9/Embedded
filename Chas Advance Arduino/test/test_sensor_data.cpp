/**
 * @file test_sensor_data.cpp
 * @brief Unit tests for sensor data validation functions
 *
 * Tests the checkThresholds function which validates sensor readings
 * against configured temperature and humidity thresholds, and sets
 * appropriate error flags when thresholds are exceeded.
 *
 * To run: pio test
 */

#include <Arduino.h>
#include <unity.h>
#include "sensorData.h"
#include "thresholds.h"

/**
 * Test that valid sensor data within thresholds passes validation
 * Using ROOM_TEMP thresholds: temp 18-25°C, humidity 30-70%
 */
void test_valid_sensor_data(void)
{
    SensorData data = {1, 0, 22.0, 50.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool exceeded = checkThresholds(data, thresholds);

    TEST_ASSERT_FALSE(exceeded);
    TEST_ASSERT_FALSE(data.error);
    TEST_ASSERT_EQUAL(NONE, data.errorType);
}

/**
 * Test that temperature below minimum is detected
 * Using ROOM_TEMP thresholds: min temp 18°C
 */
void test_temperature_too_low(void)
{
    SensorData data = {1, 0, 15.0, 50.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool exceeded = checkThresholds(data, thresholds);

    TEST_ASSERT_TRUE(exceeded);
    TEST_ASSERT_TRUE(data.error);
    TEST_ASSERT_EQUAL(TOO_LOW, data.errorType);
}

/**
 * Test that temperature above maximum is detected
 * Using ROOM_TEMP thresholds: max temp 25°C
 */
void test_temperature_too_high(void)
{
    SensorData data = {1, 0, 28.0, 50.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool exceeded = checkThresholds(data, thresholds);

    TEST_ASSERT_TRUE(exceeded);
    TEST_ASSERT_TRUE(data.error);
    TEST_ASSERT_EQUAL(TOO_HIGH, data.errorType);
}

/**
 * Test that humidity below minimum is detected
 * Using ROOM_TEMP thresholds: min humidity 30%
 */
void test_humidity_too_low(void)
{
    SensorData data = {1, 0, 22.0, 25.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool exceeded = checkThresholds(data, thresholds);

    TEST_ASSERT_TRUE(exceeded);
    TEST_ASSERT_TRUE(data.error);
    TEST_ASSERT_EQUAL(TOO_LOW, data.errorType);
}

/**
 * Test that humidity above maximum is detected
 * Using ROOM_TEMP thresholds: max humidity 70%
 */
void test_humidity_too_high(void)
{
    SensorData data = {1, 0, 22.0, 80.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool exceeded = checkThresholds(data, thresholds);

    TEST_ASSERT_TRUE(exceeded);
    TEST_ASSERT_TRUE(data.error);
    TEST_ASSERT_EQUAL(TOO_HIGH, data.errorType);
}

/**
 * Test boundary values - temperature at exact minimum
 */
void test_temperature_at_minimum(void)
{
    SensorData data = {1, 0, 18.0, 50.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool exceeded = checkThresholds(data, thresholds);

    TEST_ASSERT_FALSE(exceeded);
    TEST_ASSERT_FALSE(data.error);
}

/**
 * Test boundary values - temperature at exact maximum
 */
void test_temperature_at_maximum(void)
{
    SensorData data = {1, 0, 25.0, 50.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool exceeded = checkThresholds(data, thresholds);

    TEST_ASSERT_FALSE(exceeded);
    TEST_ASSERT_FALSE(data.error);
}

// setup() and loop() moved to test_main.cpp
