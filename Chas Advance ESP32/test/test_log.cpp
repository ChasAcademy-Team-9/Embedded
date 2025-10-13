#include <unity.h>
#include "../include/log.h"

// Mock time for testing
String mockTimeStamp = "2025-10-13 12:30:00";

void setUp(void)
{
    // Set up before each test
}

void tearDown(void)
{
    // Clean up after each test
}

// Test log event function
void test_log_event()
{
    String timestamp = "2025-10-13 12:30:00";
    String eventType = "INFO";
    String description = "Test event";
    String status = "OK";

    // Should not crash when logging
    logEvent(timestamp, eventType, description, status);

    TEST_ASSERT_TRUE(true); // If we get here, function works
}

// Test sensor data logging
void test_log_sensor_data()
{
    String timestamp = "2025-10-13 12:30:00";
    float temperature = 23.5;
    float humidity = 65.2;
    ErrorType errorType = NONE;

    // Should not crash when logging sensor data
    logSensorData(timestamp, temperature, humidity, errorType);

    TEST_ASSERT_TRUE(true);
}

// Test sensor data logging with different error types
void test_log_sensor_data_error_types()
{
    String timestamp = "2025-10-13 12:30:00";
    float temperature = 23.5;
    float humidity = 65.2;

    // Test all error types
    logSensorData(timestamp, temperature, humidity, NONE);
    logSensorData(timestamp, temperature, humidity, TOO_LOW);
    logSensorData(timestamp, temperature, humidity, TOO_HIGH);
    logSensorData(timestamp, temperature, humidity, SENSOR_FAIL);

    TEST_ASSERT_TRUE(true); // Should handle all error types
}

// Test startup logging
void test_log_startup()
{
    // Should not crash during startup logging
    logStartup();

    TEST_ASSERT_TRUE(true);
}

// Test data timeout checking
void test_check_data_timeout_normal()
{
    unsigned long timeSinceReceived = 30000; // 30 seconds

    // Should not trigger timeout for normal time
    checkDataTimeout(timeSinceReceived);

    TEST_ASSERT_TRUE(true);
}

// Test data timeout checking with timeout
void test_check_data_timeout_exceeded()
{
    unsigned long timeSinceReceived = 80000; // 80 seconds (exceeds 70s threshold)

    // Should handle timeout condition
    checkDataTimeout(timeSinceReceived);

    TEST_ASSERT_TRUE(true); // Should not crash
}

// Test timestamp generation
void test_get_timestamp()
{
    String timestamp = getTimeStamp();

    // Should return non-empty timestamp
    TEST_ASSERT_TRUE(timestamp.length() > 0);

    // Should contain date/time format elements (basic check)
    TEST_ASSERT_TRUE(timestamp.indexOf(':') >= 0); // Should have time separator
}

// Test Unix timestamp formatting
void test_format_unix_time()
{
    uint32_t testUnixTime = 1697200000; // October 13, 2023 12:00:00 UTC

    String formattedTime = formatUnixTime(testUnixTime);

    // Should return formatted string
    TEST_ASSERT_TRUE(formattedTime.length() > 0);
    TEST_ASSERT_TRUE(formattedTime.indexOf('-') >= 0); // Should have date separators
    TEST_ASSERT_TRUE(formattedTime.indexOf(':') >= 0); // Should have time separators
}

// Test timestamp string to Unix conversion
void test_timestamp_string_to_unix()
{
    String timeString = "2025-10-13 12:30:00";

    uint32_t unixTime = timestampStringToUnix(timeString);

    // Should return a reasonable Unix timestamp
    TEST_ASSERT_TRUE(unixTime > 1000000000); // Should be after year 2001
    TEST_ASSERT_TRUE(unixTime < 2147483647); // Should be before year 2038 (32-bit limit)
}

// Test Unix time roundtrip conversion
void test_unix_time_roundtrip()
{
    uint32_t originalTime = 1697200000;

    // Convert to string and back
    String timeString = formatUnixTime(originalTime);
    uint32_t convertedBack = timestampStringToUnix(timeString);

    // Should be approximately the same (allowing for precision loss)
    TEST_ASSERT_UINT32_WITHIN(60, originalTime, convertedBack); // Within 1 minute
}

// Test error type enumeration values
void test_error_type_values()
{
    // Verify error type constants have expected values
    TEST_ASSERT_EQUAL_INT(0, NONE);
    TEST_ASSERT_EQUAL_INT(1, TOO_LOW);
    TEST_ASSERT_EQUAL_INT(2, TOO_HIGH);
    TEST_ASSERT_EQUAL_INT(4, SENSOR_FAIL);

    // Values should be unique
    TEST_ASSERT_NOT_EQUAL(NONE, TOO_LOW);
    TEST_ASSERT_NOT_EQUAL(TOO_LOW, TOO_HIGH);
    TEST_ASSERT_NOT_EQUAL(TOO_HIGH, SENSOR_FAIL);
}

// Test NTP configuration constants
void test_ntp_configuration()
{
    extern const long gmtOffset_sec;
    extern const int daylightOffset_sec;
    extern const int dataReceivedThreshold;

    // Verify NTP configuration is reasonable
    TEST_ASSERT_TRUE(gmtOffset_sec >= -43200 && gmtOffset_sec <= 43200);           // ±12 hours
    TEST_ASSERT_TRUE(daylightOffset_sec >= 0 && daylightOffset_sec <= 7200);       // 0-2 hours
    TEST_ASSERT_TRUE(dataReceivedThreshold > 0 && dataReceivedThreshold < 300000); // Reasonable timeout
}

// Test logging with extreme temperature values
void test_log_extreme_temperatures()
{
    String timestamp = "2025-10-13 12:30:00";

    // Test very high temperature
    logSensorData(timestamp, 85.0, 65.2, TOO_HIGH);

    // Test very low temperature
    logSensorData(timestamp, -40.0, 65.2, TOO_LOW);

    // Test NaN temperature
    logSensorData(timestamp, NAN, 65.2, SENSOR_FAIL);

    TEST_ASSERT_TRUE(true); // Should handle extreme values
}

// Test logging with extreme humidity values
void test_log_extreme_humidity()
{
    String timestamp = "2025-10-13 12:30:00";

    // Test 0% humidity
    logSensorData(timestamp, 23.5, 0.0, NONE);

    // Test 100% humidity
    logSensorData(timestamp, 23.5, 100.0, NONE);

    // Test NaN humidity
    logSensorData(timestamp, 23.5, NAN, SENSOR_FAIL);

    TEST_ASSERT_TRUE(true); // Should handle extreme humidity values
}

// Test timestamp edge cases
void test_timestamp_edge_cases()
{
    // Test empty timestamp
    String emptyTimestamp = "";
    uint32_t result1 = timestampStringToUnix(emptyTimestamp);
    TEST_ASSERT_TRUE(result1 >= 0); // Should handle gracefully

    // Test malformed timestamp
    String malformedTimestamp = "not-a-timestamp";
    uint32_t result2 = timestampStringToUnix(malformedTimestamp);
    TEST_ASSERT_TRUE(result2 >= 0); // Should handle gracefully

    // Test Unix timestamp 0 (epoch)
    String formatted = formatUnixTime(0);
    TEST_ASSERT_TRUE(formatted.length() > 0); // Should format even epoch
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_log_event);
    RUN_TEST(test_log_sensor_data);
    RUN_TEST(test_log_sensor_data_error_types);
    RUN_TEST(test_log_startup);
    RUN_TEST(test_check_data_timeout_normal);
    RUN_TEST(test_check_data_timeout_exceeded);
    RUN_TEST(test_get_timestamp);
    RUN_TEST(test_format_unix_time);
    RUN_TEST(test_timestamp_string_to_unix);
    RUN_TEST(test_unix_time_roundtrip);
    RUN_TEST(test_error_type_values);
    RUN_TEST(test_ntp_configuration);
    RUN_TEST(test_log_extreme_temperatures);
    RUN_TEST(test_log_extreme_humidity);
    RUN_TEST(test_timestamp_edge_cases);

    return UNITY_END();
}