/**
 * @file test_main.cpp
 * @brief Main entry point for Unity tests
 */

#include <unity.h>

#ifdef NATIVE_BUILD
#include "Arduino.h"
// Define the global Serial mock instance
SerialMock Serial;
#endif

// External test function declarations
extern void test_mock_json_format(void);
extern void test_mock_json_temperature_range(void);
extern void test_mock_json_error_values(void);
extern void test_serialize_batch_to_json(void);
extern void test_serialize_empty_batch(void);
extern void test_timestamp_string_to_unix(void);
extern void test_format_unix_time(void);
extern void test_error_type_enum(void);

// Unity requires these functions to be defined globally
void setUp(void)
{
    // Global setup for all tests - currently unused
}

void tearDown(void)
{
    // Global teardown for all tests - currently unused
}

int main()
{
    UNITY_BEGIN();

    // Mock JSON tests
    RUN_TEST(test_mock_json_format);
    RUN_TEST(test_mock_json_temperature_range);
    RUN_TEST(test_mock_json_error_values);

    // JSON parser tests
    RUN_TEST(test_serialize_batch_to_json);
    RUN_TEST(test_serialize_empty_batch);

    // Log function tests
    RUN_TEST(test_timestamp_string_to_unix);
    RUN_TEST(test_format_unix_time);
    RUN_TEST(test_error_type_enum);

    return UNITY_END();
}