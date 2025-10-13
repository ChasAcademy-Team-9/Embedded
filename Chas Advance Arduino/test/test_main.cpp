#include <unity.h>

// Include all test functions
extern "C"
{
    // From test_sensor_data.cpp
    extern int test_sensor_data_main();

    // From test_batch_handler.cpp
    extern int test_batch_handler_main();

    // From test_arduino_logger.cpp
    extern int test_arduino_logger_main();

    // From test_wifi_handler.cpp
    extern int test_wifi_handler_main();

    // From test_integration.cpp
    extern int test_integration_main();

    // From test_mock_sensor.cpp
    extern int test_mock_sensor_main();
}

void setUp(void)
{
    // Global setup before each test suite
}

void tearDown(void)
{
    // Global cleanup after each test suite
}

// Main test runner
int main()
{
    UNITY_BEGIN();

    Serial.println("\n=== Arduino Project Test Suite ===\n");

    Serial.println("Running Sensor Data Tests...");
    test_sensor_data_main();

    Serial.println("\nRunning Batch Handler Tests...");
    test_batch_handler_main();

    Serial.println("\nRunning Arduino Logger Tests...");
    test_arduino_logger_main();

    Serial.println("\nRunning WiFi Handler Tests...");
    test_wifi_handler_main();

    Serial.println("\nRunning Mock Sensor Tests...");
    test_mock_sensor_main();

    Serial.println("\nRunning Integration Tests...");
    test_integration_main();

    Serial.println("\n=== All Tests Complete ===");

    return UNITY_END();
}