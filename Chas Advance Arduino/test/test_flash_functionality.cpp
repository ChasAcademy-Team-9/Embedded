/**
 * @file test_flash_functionality.cpp
 * @brief Unit tests for Arduino flash data functionality
 *
 * Tests the new flash data retrieval and sending capabilities
 * added in the development merge.
 */

#include <unity.h>
#include "arduinoLogger.h"
#include "sensorData.h"

// Use the global logger instance declared in test_main.cpp
extern Logger logger;

void test_get_flash_data_as_batch_empty(void)
{
    logger.begin();
    logger.clearAll(); // Ensure flash is empty
    
    std::vector<SensorData> flashBatch = logger.getFlashDataAsBatch(1);
    
    TEST_ASSERT_EQUAL(0, flashBatch.size());
}

void test_get_flash_data_as_batch_with_data(void)
{
    logger.begin();
    logger.clearAll();
    
    // Add some test data to flash
    logger.log("25.5,60.0,0"); // Temperature, humidity, no error
    logger.log("26.0,65.0,1"); // Temperature, humidity, with error
    
    std::vector<SensorData> flashBatch = logger.getFlashDataAsBatch(1);
    
    TEST_ASSERT_EQUAL(2, flashBatch.size());
    
    // Check first entry
    TEST_ASSERT_EQUAL_FLOAT(25.5, flashBatch[0].temperature);
    TEST_ASSERT_EQUAL_FLOAT(60.0, flashBatch[0].humidity);
    TEST_ASSERT_EQUAL(1, flashBatch[0].SensorId);
    TEST_ASSERT_EQUAL(0, flashBatch[0].errorType);
    TEST_ASSERT_FALSE(flashBatch[0].error);
    
    // Check second entry
    TEST_ASSERT_EQUAL_FLOAT(26.0, flashBatch[1].temperature);
    TEST_ASSERT_EQUAL_FLOAT(65.0, flashBatch[1].humidity);
    TEST_ASSERT_EQUAL(1, flashBatch[1].SensorId);
    TEST_ASSERT_EQUAL(1, flashBatch[1].errorType);
    TEST_ASSERT_TRUE(flashBatch[1].error);
    
    logger.clearAll(); // Clean up
}

void test_send_flash_data_when_empty(void)
{
    logger.begin();
    logger.clearAll(); // Ensure flash is empty
    
    bool result = logger.sendFlashDataIfAvailable(1);
    
    TEST_ASSERT_FALSE(result); // Should return false when no data to send
}

void test_flash_data_parsing_malformed_entries(void)
{
    logger.begin();
    logger.clearAll();
    
    // Add malformed entry (missing comma)
    logger.log("25.5-60.0-0");
    
    std::vector<SensorData> flashBatch = logger.getFlashDataAsBatch(1);
    
    // Should handle malformed entries gracefully (skip them)
    TEST_ASSERT_EQUAL(0, flashBatch.size());
    
    logger.clearAll(); // Clean up
}

void test_flash_data_preserves_sensor_id(void)
{
    logger.begin();
    logger.clearAll();
    
    logger.log("22.0,55.0,0");
    
    std::vector<SensorData> flashBatch = logger.getFlashDataAsBatch(5); // Use different sensor ID
    
    TEST_ASSERT_EQUAL(1, flashBatch.size());
    TEST_ASSERT_EQUAL(5, flashBatch[0].SensorId); // Should use the provided sensor ID
    
    logger.clearAll(); // Clean up
}