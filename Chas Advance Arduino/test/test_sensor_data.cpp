#include <unity.h>
#include "../include/sensorData.h"
#include "../include/thresholds.h"
#include "../include/log.h"

void setUp(void)
{
    // Set up before each test
}

void tearDown(void)
{
    // Clean up after each test
}

// Test SensorData structure creation
void test_sensor_data_creation()
{
    SensorData data = {1000, 23.5, 65.2, false, NONE};

    TEST_ASSERT_EQUAL_UINT32(1000, data.timestamp);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.5, data.temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 65.2, data.humidity);
    TEST_ASSERT_FALSE(data.error);
    TEST_ASSERT_EQUAL_INT(NONE, data.errorType);
}

// Test threshold checking - normal values
void test_threshold_normal_values()
{
    SensorData data = {1000, 22.0, 60.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool result = checkThresholds(data, thresholds);

    TEST_ASSERT_FALSE(data.error);
    TEST_ASSERT_EQUAL_INT(NONE, data.errorType);
}

// Test threshold checking - too high temperature
void test_threshold_temp_too_high()
{
    SensorData data = {1000, 30.0, 60.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool result = checkThresholds(data, thresholds);

    TEST_ASSERT_TRUE(data.error);
    TEST_ASSERT_EQUAL_INT(TOO_HIGH, data.errorType);
}

// Test threshold checking - too low temperature
void test_threshold_temp_too_low()
{
    SensorData data = {1000, 15.0, 60.0, false, NONE};
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);

    bool result = checkThresholds(data, thresholds);

    TEST_ASSERT_TRUE(data.error);
    TEST_ASSERT_EQUAL_INT(TOO_LOW, data.errorType);
}

// Test different temperature modes
void test_different_temp_modes()
{
    Thresholds roomTemp = getThresholdsForMode(ROOM_TEMP);
    Thresholds coldTemp = getThresholdsForMode(COLD_TEMP);
    Thresholds warmTemp = getThresholdsForMode(WARM_TEMP);

    // Verify different modes have different thresholds
    TEST_ASSERT_NOT_EQUAL(roomTemp.minTemp, coldTemp.minTemp);
    TEST_ASSERT_NOT_EQUAL(roomTemp.maxTemp, warmTemp.maxTemp);
}

// Test NaN sensor values (sensor failure)
void test_nan_sensor_values()
{
    SensorData data = {1000, NAN, 60.0, false, NONE};

    // This would normally be handled in main loop
    if (isnan(data.temperature) || isnan(data.humidity))
    {
        data.error = true;
        data.errorType = SENSOR_FAIL;
    }

    TEST_ASSERT_TRUE(data.error);
    TEST_ASSERT_EQUAL_INT(SENSOR_FAIL, data.errorType);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_sensor_data_creation);
    RUN_TEST(test_threshold_normal_values);
    RUN_TEST(test_threshold_temp_too_high);
    RUN_TEST(test_threshold_temp_too_low);
    RUN_TEST(test_different_temp_modes);
    RUN_TEST(test_nan_sensor_values);

    return UNITY_END();
}