/**
 * @file test_main.cpp
 * @brief Main entry point for Unity tests
 */

#include <unity.h>

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

    return UNITY_END();
}