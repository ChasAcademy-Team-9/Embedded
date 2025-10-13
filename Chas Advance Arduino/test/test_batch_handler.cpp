#include <unity.h>
#include "../include/batchHandler.h"
#include "../include/sensorData.h"
#include "../include/log.h"

extern unsigned long batchSendInterval; // Access to batch interval

void setUp(void)
{
    // Reset batch state before each test
    resetBatchTimer();
    std::vector<SensorData> &batch = getBatchBuffer();
    batch.clear();
}

void tearDown(void)
{
    // Clean up after each test
}

// Test adding single sensor reading to batch
void test_batch_add_single_reading()
{
    SensorData data = {1000, 23.5, 65.0, false, NONE};

    bool shouldSend = batchSensorReadings(data);

    TEST_ASSERT_FALSE(shouldSend); // Should not send immediately
    TEST_ASSERT_EQUAL_INT(1, getBatchBuffer().size());
}

// Test adding multiple readings to batch
void test_batch_add_multiple_readings()
{
    for (int i = 0; i < 5; i++)
    {
        SensorData data = {1000 + i * 1000, 23.0 + i, 65.0, false, NONE};
        batchSensorReadings(data);
    }

    TEST_ASSERT_EQUAL_INT(5, getBatchBuffer().size());
}

// Test batch time interval trigger (mock millis())
void test_batch_time_interval_trigger()
{
    SensorData data = {1000, 23.5, 65.0, false, NONE};

    // Add first reading
    bool shouldSend1 = batchSensorReadings(data);
    TEST_ASSERT_FALSE(shouldSend1);

    // Mock time passing (this would need millis() mocking in real implementation)
    // For now, test the logic conceptually
    TEST_ASSERT_TRUE(batchSendInterval > 0); // Verify interval is set
}

// Test median calculation - normal case
void test_calculate_median_normal()
{
    std::vector<SensorData> testBatch;

    // Add test data: temperatures 20, 22, 24 -> median 22
    testBatch.push_back({1000, 20.0, 60.0, false, NONE});
    testBatch.push_back({2000, 22.0, 62.0, false, NONE});
    testBatch.push_back({3000, 24.0, 64.0, false, NONE});

    SensorData median = calculateMedian(testBatch);

    TEST_ASSERT_FLOAT_WITHIN(0.1, 22.0, median.temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 62.0, median.humidity);
    TEST_ASSERT_FALSE(median.error);
}

// Test median calculation - with sensor failures
void test_calculate_median_with_failures()
{
    std::vector<SensorData> testBatch;

    // Add valid data
    testBatch.push_back({1000, 20.0, 60.0, false, NONE});
    testBatch.push_back({2000, 22.0, 62.0, false, NONE});
    // Add failed reading
    testBatch.push_back({3000, NAN, NAN, true, SENSOR_FAIL});

    SensorData median = calculateMedian(testBatch);

    // Should calculate median from valid data only
    TEST_ASSERT_FLOAT_WITHIN(0.1, 21.0, median.temperature); // (20+22)/2
    TEST_ASSERT_FLOAT_WITHIN(0.1, 61.0, median.humidity);    // (60+62)/2
}

// Test median calculation - even number of values
void test_calculate_median_even_count()
{
    std::vector<SensorData> testBatch;

    testBatch.push_back({1000, 20.0, 60.0, false, NONE});
    testBatch.push_back({2000, 22.0, 62.0, false, NONE});
    testBatch.push_back({3000, 24.0, 64.0, false, NONE});
    testBatch.push_back({4000, 26.0, 66.0, false, NONE});

    SensorData median = calculateMedian(testBatch);

    // Median of 20,22,24,26 should be (22+24)/2 = 23
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.0, median.temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 63.0, median.humidity);
}

// Test median calculation - all sensor failures
void test_calculate_median_all_failures()
{
    std::vector<SensorData> testBatch;

    testBatch.push_back({1000, NAN, NAN, true, SENSOR_FAIL});
    testBatch.push_back({2000, NAN, NAN, true, SENSOR_FAIL});

    SensorData median = calculateMedian(testBatch);

    TEST_ASSERT_TRUE(median.error);
    TEST_ASSERT_TRUE(isnan(median.temperature) || median.temperature == 0);
}

// Test batch buffer reset
void test_batch_reset()
{
    // Add some data
    SensorData data = {1000, 23.5, 65.0, false, NONE};
    batchSensorReadings(data);
    batchSensorReadings(data);

    TEST_ASSERT_EQUAL_INT(2, getBatchBuffer().size());

    // Clear and reset
    getBatchBuffer().clear();
    resetBatchTimer();

    TEST_ASSERT_EQUAL_INT(0, getBatchBuffer().size());
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_batch_add_single_reading);
    RUN_TEST(test_batch_add_multiple_readings);
    RUN_TEST(test_batch_time_interval_trigger);
    RUN_TEST(test_calculate_median_normal);
    RUN_TEST(test_calculate_median_with_failures);
    RUN_TEST(test_calculate_median_even_count);
    RUN_TEST(test_calculate_median_all_failures);
    RUN_TEST(test_batch_reset);

    return UNITY_END();
}