/**
 * @file test_log_functions.cpp
 * @brief Unit tests for log utility functions in native environment
 */

#include <unity.h>

#ifdef NATIVE_BUILD
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

#include "log.h"

void test_timestamp_string_to_unix(void)
{
    // Test: Verify timestamp string conversion to Unix timestamp format
    String testTime = "2025-01-01 12:00:00";
    uint32_t result = timestampStringToUnix(testTime);

    // Should return a reasonable unix timestamp (not 0)
    TEST_ASSERT_TRUE(result > 0);
}

void test_format_unix_time(void)
{
    // Test: Verify Unix timestamp formatting to readable string format
    uint32_t testUnix = 1704110400; // 2024-01-01 12:00:00 UTC
    String result = formatUnixTime(testUnix);

    // Should return a formatted string (not empty)
    TEST_ASSERT_TRUE(result.length() > 0);
    TEST_ASSERT_TRUE(result.indexOf("2024") >= 0);
}

void test_error_type_enum(void)
{
    // Test: Verify error type enumeration values are correctly defined
    TEST_ASSERT_EQUAL(0, NONE);
    TEST_ASSERT_EQUAL(1, TOO_LOW);
    TEST_ASSERT_EQUAL(2, TOO_HIGH);
    TEST_ASSERT_EQUAL(4, SENSOR_FAIL);
}

// Test functions are automatically discovered by Unity