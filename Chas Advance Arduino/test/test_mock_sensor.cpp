#include <unity.h>
#include "../include/mockSensor.h"

void setUp(void)
{
    // Set up before each test
}

void tearDown(void)
{
    // Clean up after each test
}

// Test mock sensor value ranges
void test_mock_sensor_ranges()
{
    // Initialize mock sensor if needed
    initMockSensor();

    // Get multiple readings to test range
    for (int i = 0; i < 10; i++)
    {
        float temp = readMockTemperature();
        float humidity = readMockHumidity();

        // Verify reasonable ranges
        TEST_ASSERT_TRUE(temp >= -40.0 && temp <= 80.0);        // Reasonable temp range
        TEST_ASSERT_TRUE(humidity >= 0.0 && humidity <= 100.0); // Humidity percentage
    }
}

// Test mock sensor error simulation
void test_mock_sensor_errors()
{
    // Test if mock sensor can simulate failures
    simulateSensorFailure(true);

    float temp = readMockTemperature();
    float humidity = readMockHumidity();

    // Should return NaN or error values
    TEST_ASSERT_TRUE(isnan(temp) || temp == -999.0);
    TEST_ASSERT_TRUE(isnan(humidity) || humidity == -999.0);

    // Reset to normal operation
    simulateSensorFailure(false);

    temp = readMockTemperature();
    humidity = readMockHumidity();

    // Should return valid values
    TEST_ASSERT_FALSE(isnan(temp));
    TEST_ASSERT_FALSE(isnan(humidity));
}

// Test mock sensor consistency
void test_mock_sensor_consistency()
{
    initMockSensor();

    float temp1 = readMockTemperature();
    float temp2 = readMockTemperature();

    // Values should change (not static)
    // But should be within reasonable variation
    float diff = abs(temp1 - temp2);
    TEST_ASSERT_TRUE(diff < 10.0); // Shouldn't jump more than 10 degrees
}

// Test mock sensor initialization
void test_mock_sensor_initialization()
{
    // Test that initialization works without errors
    initMockSensor();

    // Should be able to read values after init
    float temp = readMockTemperature();
    float humidity = readMockHumidity();

    // Values should be numbers (not NaN initially)
    TEST_ASSERT_FALSE(isnan(temp));
    TEST_ASSERT_FALSE(isnan(humidity));
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_mock_sensor_ranges);
    RUN_TEST(test_mock_sensor_errors);
    RUN_TEST(test_mock_sensor_consistency);
    RUN_TEST(test_mock_sensor_initialization);

    return UNITY_END();
}