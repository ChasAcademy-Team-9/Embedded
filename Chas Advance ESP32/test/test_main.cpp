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
extern void test_median_odd_values(void);
extern void test_median_even_values(void);
extern void test_median_empty_vector(void);
extern void test_calc_median_excludes_errors(void);
extern void test_serialize_batch_to_json(void);
extern void test_serialize_empty_batch(void);
extern void test_parse_json_valid_data(void);
extern void test_parse_json_invalid_data(void);
extern void test_parse_json_missing_fields(void);
extern void test_parse_json_array_functionality(void);
extern void test_timestamp_string_to_unix(void);
extern void test_format_unix_time(void);
extern void test_error_type_enum(void);
extern void test_error_handling_null_json(void);
extern void test_error_handling_malformed_json(void);
extern void test_error_handling_wrong_data_types(void);
extern void test_error_handling_empty_sensor_batch(void);
extern void test_error_handling_median_with_all_errors(void);
extern void test_error_handling_median_empty_vector(void);

int main()
{
    UNITY_BEGIN();

    // Mock JSON tests
    RUN_TEST(test_mock_json_format);
    RUN_TEST(test_mock_json_temperature_range);
    RUN_TEST(test_mock_json_error_values);

    // Sensor data handler tests
    RUN_TEST(test_median_odd_values);
    RUN_TEST(test_median_even_values);
    RUN_TEST(test_median_empty_vector);
    RUN_TEST(test_calc_median_excludes_errors);

    // JSON parser tests
    RUN_TEST(test_serialize_batch_to_json);
    RUN_TEST(test_serialize_empty_batch);
    RUN_TEST(test_parse_json_valid_data);
    RUN_TEST(test_parse_json_invalid_data);
    RUN_TEST(test_parse_json_missing_fields);
    RUN_TEST(test_parse_json_array_functionality);

    // Log function tests
    RUN_TEST(test_timestamp_string_to_unix);
    RUN_TEST(test_format_unix_time);
    RUN_TEST(test_error_type_enum);

    // Error handling tests
    RUN_TEST(test_error_handling_null_json);
    RUN_TEST(test_error_handling_malformed_json);
    RUN_TEST(test_error_handling_wrong_data_types);
    RUN_TEST(test_error_handling_empty_sensor_batch);
    RUN_TEST(test_error_handling_median_with_all_errors);
    RUN_TEST(test_error_handling_median_empty_vector);

    return UNITY_END();
}