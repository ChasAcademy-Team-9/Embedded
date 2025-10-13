#include <unity.h>
#include "../include/sensorData.h"
#include "../include/batchHandler.h"
#include "../include/arduinoLogger.h"
#include "../include/wifiHandler.h"
#include "../include/log.h"

Logger integrationLogger;

void setUp(void)
{
    // Reset all systems before each test
    integrationLogger.clearAll();
    integrationLogger.begin();

    std::vector<SensorData> &batch = getBatchBuffer();
    batch.clear();
    resetBatchTimer();
}

void tearDown(void)
{
    // Clean up after each test
}

// Test complete sensor reading to batch workflow
void test_sensor_to_batch_workflow()
{
    // Simulate sensor reading
    SensorData reading1 = {1000, 23.5, 65.0, false, NONE};
    SensorData reading2 = {2000, 24.0, 66.0, false, NONE};
    SensorData reading3 = {3000, 22.5, 64.0, false, NONE};

    // Add readings to batch
    batchSensorReadings(reading1);
    batchSensorReadings(reading2);
    batchSensorReadings(reading3);

    // Verify batch contains all readings
    std::vector<SensorData> &batch = getBatchBuffer();
    TEST_ASSERT_EQUAL_INT(3, batch.size());

    // Calculate median
    SensorData median = calculateMedian(batch);

    // Verify median calculation (middle value of 22.5, 23.5, 24.0 = 23.5)
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.5, median.temperature);
}

// Test error handling workflow
void test_error_handling_workflow()
{
    // Create reading with sensor failure
    SensorData errorReading = {1000, NAN, NAN, true, SENSOR_FAIL};

    // Add to batch
    batchSensorReadings(errorReading);

    // Add some normal readings
    batchSensorReadings({2000, 23.5, 65.0, false, NONE});
    batchSensorReadings({3000, 24.0, 66.0, false, NONE});

    // Calculate median (should handle error data correctly)
    std::vector<SensorData> &batch = getBatchBuffer();
    SensorData median = calculateMedian(batch);

    // Should calculate from valid data only
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.75, median.temperature); // (23.5+24.0)/2
    TEST_ASSERT_FALSE(median.error);                          // Median should not be in error state
}

// Test offline logging workflow
void test_offline_logging_workflow()
{
    // Simulate failed batch send by adding data to flash log
    SensorData medianData = {5000, 23.5, 65.2, false, NONE};

    integrationLogger.logMedian(medianData);

    TEST_ASSERT_EQUAL_INT(1, integrationLogger.size());
    TEST_ASSERT_EQUAL_STRING("23.5,65.2,0", integrationLogger.getEntry(0).c_str());

    // Test log clearing after successful send
    integrationLogger.clearAll();
    TEST_ASSERT_EQUAL_INT(0, integrationLogger.size());
}

// Test threshold detection integration
void test_threshold_integration()
{
    // Create reading that should trigger threshold error
    SensorData highTempReading = {1000, 30.0, 65.0, false, NONE};

    // Check thresholds
    Thresholds thresholds = getThresholdsForMode(ROOM_TEMP);
    checkThresholds(highTempReading, thresholds);

    // Should be marked as error
    TEST_ASSERT_TRUE(highTempReading.error);
    TEST_ASSERT_EQUAL_INT(TOO_HIGH, highTempReading.errorType);

    // Add to batch and verify error handling
    batchSensorReadings(highTempReading);

    std::vector<SensorData> &batch = getBatchBuffer();
    TEST_ASSERT_EQUAL_INT(1, batch.size());
    TEST_ASSERT_TRUE(batch[0].error);
}

// Test batch to flash log integration
void test_batch_to_flash_integration()
{
    // Create batch with mixed data
    SensorData normal1 = {1000, 22.0, 60.0, false, NONE};
    SensorData error1 = {2000, 35.0, 70.0, true, TOO_HIGH};
    SensorData normal2 = {3000, 23.0, 62.0, false, NONE};

    std::vector<SensorData> testBatch = {normal1, error1, normal2};

    // Test createLogFromBatch functionality
    integrationLogger.createLogFromBatch(testBatch, millis());

    // Should have logged either first error or median
    TEST_ASSERT_TRUE(integrationLogger.size() > 0);

    String logEntry = integrationLogger.getEntry(0);

    // Should contain valid data (either error or median)
    TEST_ASSERT_TRUE(logEntry.indexOf(',') > 0); // Contains comma separator
    TEST_ASSERT_TRUE(logEntry.length() > 5);     // Has meaningful content
}

// Test system recovery after connection restore
void test_connection_recovery_integration()
{
    // Simulate offline operation - add data to flash
    integrationLogger.log("22.5,60.0,0");
    integrationLogger.log("23.0,61.0,1");
    integrationLogger.log("23.5,62.0,0");

    TEST_ASSERT_EQUAL_INT(3, integrationLogger.size());

    // Simulate connection recovery and successful send
    String flashLogData = "";
    for (size_t i = 0; i < integrationLogger.size(); i++)
    {
        flashLogData += integrationLogger.getEntry(i);
        if (i < integrationLogger.size() - 1)
        {
            flashLogData += "\n";
        }
    }

    // Verify log data format for sending
    TEST_ASSERT_TRUE(flashLogData.indexOf("22.5,60.0,0") >= 0);
    TEST_ASSERT_TRUE(flashLogData.indexOf('\n') > 0);

    // Simulate successful send - clear logs
    integrationLogger.clearAll();
    TEST_ASSERT_EQUAL_INT(0, integrationLogger.size());
}

// Test memory management under load
void test_memory_management_load()
{
    // Add many readings to test memory handling
    for (int i = 0; i < 100; i++)
    {
        SensorData reading = {1000 + i * 1000, 20.0 + i * 0.1, 60.0, false, NONE};
        batchSensorReadings(reading);

        // Prevent actual time-based sending by resetting timer
        if (i % 10 == 0)
        {
            resetBatchTimer();
        }
    }

    // System should handle large batch gracefully
    std::vector<SensorData> &batch = getBatchBuffer();
    TEST_ASSERT_TRUE(batch.size() > 0);

    // Calculate median should work even with large dataset
    SensorData median = calculateMedian(batch);
    TEST_ASSERT_FALSE(isnan(median.temperature));
    TEST_ASSERT_FALSE(isnan(median.humidity));
}

// Test complete system state consistency
void test_system_state_consistency()
{
    // Verify initial state
    TEST_ASSERT_EQUAL_INT(0, getBatchBuffer().size());
    TEST_ASSERT_EQUAL_INT(0, integrationLogger.size());

    // Perform operations
    SensorData reading = {1000, 23.5, 65.0, false, NONE};
    batchSensorReadings(reading);
    integrationLogger.log("23.5,65.0,0");

    // Verify state changes
    TEST_ASSERT_EQUAL_INT(1, getBatchBuffer().size());
    TEST_ASSERT_EQUAL_INT(1, integrationLogger.size());

    // Reset and verify clean state
    getBatchBuffer().clear();
    integrationLogger.clearAll();

    TEST_ASSERT_EQUAL_INT(0, getBatchBuffer().size());
    TEST_ASSERT_EQUAL_INT(0, integrationLogger.size());
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_sensor_to_batch_workflow);
    RUN_TEST(test_error_handling_workflow);
    RUN_TEST(test_offline_logging_workflow);
    RUN_TEST(test_threshold_integration);
    RUN_TEST(test_batch_to_flash_integration);
    RUN_TEST(test_connection_recovery_integration);
    RUN_TEST(test_memory_management_load);
    RUN_TEST(test_system_state_consistency);

    return UNITY_END();
}