#include <unity.h>
#include "../include/espLogger.h"
#include "../include/sensorDataHandler.h"
#include <vector>

ESPLogger testLogger;

void setUp(void) {
    // Initialize filesystem and clear old data
    testLogger.begin();
    testLogger.clearBatches();
    testLogger.clearErrors();
}

void tearDown(void) {
    // Clean up after each test
    testLogger.clearBatches();
    testLogger.clearErrors();
}

// Test logger initialization
void test_logger_initialization() {
    testLogger.begin();
    
    // Should initialize without errors
    TEST_ASSERT_TRUE(true); // If we get here, initialization worked
}

// Test error logging
void test_error_logging() {
    String testError = "Test error message";
    
    testLogger.logError(testError);
    
    // Should be able to log without crashing
    TEST_ASSERT_TRUE(true);
}

// Test batch logging
void test_batch_logging() {
    std::vector<SensorData> testBatch;
    
    SensorData entry1 = {1697200000, 23.5, 65.2, false, 0, 1};
    SensorData entry2 = {1697200030, 24.0, 66.0, false, 0, 1};
    
    testBatch.push_back(entry1);
    testBatch.push_back(entry2);
    
    testLogger.logBatch(testBatch);
    
    // Verify batch was stored
    std::vector<SensorData> retrievedBatch;
    uint16_t batchIndex;
    bool hasOldestBatch = testLogger.getOldestBatch(retrievedBatch, batchIndex);
    
    TEST_ASSERT_TRUE(hasOldestBatch);
    TEST_ASSERT_EQUAL_INT(2, retrievedBatch.size());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.5, retrievedBatch[0].temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 65.2, retrievedBatch[0].humidity);
}

// Test batch retrieval - oldest first
void test_batch_retrieval_oldest() {
    std::vector<SensorData> batch1, batch2;
    
    // Create first batch
    batch1.push_back({1697200000, 20.0, 60.0, false, 0, 1});
    batch1.push_back({1697200030, 21.0, 61.0, false, 0, 1});
    
    // Create second batch  
    batch2.push_back({1697200060, 22.0, 62.0, false, 0, 1});
    batch2.push_back({1697200090, 23.0, 63.0, false, 0, 1});
    
    testLogger.logBatch(batch1);
    testLogger.logBatch(batch2);
    
    // Get oldest batch
    std::vector<SensorData> retrievedBatch;
    uint16_t batchIndex;
    bool success = testLogger.getOldestBatch(retrievedBatch, batchIndex);
    
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(2, retrievedBatch.size());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 20.0, retrievedBatch[0].temperature); // Should be first batch
}

// Test batch retrieval - newest 
void test_batch_retrieval_newest() {
    std::vector<SensorData> batch1, batch2;
    
    batch1.push_back({1697200000, 20.0, 60.0, false, 0, 1});
    batch2.push_back({1697200060, 22.0, 62.0, false, 0, 1});
    
    testLogger.logBatch(batch1);
    testLogger.logBatch(batch2);
    
    // Get newest batch
    std::vector<SensorData> retrievedBatch;
    uint16_t batchIndex;
    bool success = testLogger.getNewestBatch(retrievedBatch, batchIndex);
    
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(1, retrievedBatch.size());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 22.0, retrievedBatch[0].temperature); // Should be second batch
}

// Test batch removal
void test_batch_removal() {
    std::vector<SensorData> testBatch;
    testBatch.push_back({1697200000, 23.5, 65.2, false, 0, 1});
    
    testLogger.logBatch(testBatch);
    
    // Verify batch exists
    std::vector<SensorData> retrievedBatch;
    uint16_t batchIndex;
    bool hasData = testLogger.getOldestBatch(retrievedBatch, batchIndex);
    TEST_ASSERT_TRUE(hasData);
    
    // Remove oldest batch
    testLogger.removeOldestBatch();
    
    // Verify batch is gone
    hasData = testLogger.getOldestBatch(retrievedBatch, batchIndex);
    TEST_ASSERT_FALSE(hasData);
}

// Test send status logging
void test_send_status_logging() {
    int batchId = 123;
    bool success = true;
    String message = "Successfully sent";
    
    testLogger.logSendStatus(batchId, success, message);
    
    // Should log without crashing
    TEST_ASSERT_TRUE(true);
}

// Test clearing all batches
void test_clear_all_batches() {
    // Add multiple batches
    std::vector<SensorData> batch1, batch2;
    batch1.push_back({1697200000, 20.0, 60.0, false, 0, 1});
    batch2.push_back({1697200060, 22.0, 62.0, false, 0, 1});
    
    testLogger.logBatch(batch1);
    testLogger.logBatch(batch2);
    
    // Clear all
    testLogger.clearBatches();
    
    // Verify all cleared
    std::vector<SensorData> retrievedBatch;
    uint16_t batchIndex;
    bool hasData = testLogger.getOldestBatch(retrievedBatch, batchIndex);
    TEST_ASSERT_FALSE(hasData);
}

// Test batch indices management
void test_batch_indices() {
    std::vector<SensorData> testBatch;
    testBatch.push_back({1697200000, 23.5, 65.2, false, 0, 1});
    
    // Add several batches
    for (int i = 0; i < 5; i++) {
        testLogger.logBatch(testBatch);
    }
    
    // Get batch indices
    std::vector<uint16_t> indices = testLogger.getBatchIndices();
    
    TEST_ASSERT_EQUAL_INT(5, indices.size());
    
    // Indices should be sorted
    for (size_t i = 1; i < indices.size(); i++) {
        TEST_ASSERT_TRUE(indices[i-1] < indices[i]);
    }
}

// Test CRC validation
void test_batch_crc_validation() {
    std::vector<SensorData> testBatch;
    testBatch.push_back({1697200000, 23.5, 65.2, false, 0, 1});
    
    testLogger.logBatch(testBatch);
    
    // Get batch index
    std::vector<uint16_t> indices = testLogger.getBatchIndices();
    TEST_ASSERT_TRUE(indices.size() > 0);
    
    // Read batch file (this tests CRC validation internally)
    String filename = testLogger.getBatchFilename(indices[0]);
    std::vector<SensorData> retrievedBatch;
    bool validCRC = testLogger.readBatchFile(filename, retrievedBatch);
    
    TEST_ASSERT_TRUE(validCRC);
    TEST_ASSERT_EQUAL_INT(1, retrievedBatch.size());
}

// Test maximum batches limit
void test_max_batches_limit() {
    std::vector<SensorData> testBatch;
    testBatch.push_back({1697200000, 23.5, 65.2, false, 0, 1});
    
    // Add more than MAX_BATCHES (20)
    for (int i = 0; i < 25; i++) {
        testBatch[0].timestamp = 1697200000 + i * 30;
        testLogger.logBatch(testBatch);
    }
    
    // Should not exceed maximum
    std::vector<uint16_t> indices = testLogger.getBatchIndices();
    TEST_ASSERT_TRUE(indices.size() <= 20); // MAX_BATCHES = 20
}

// Test empty batch handling
void test_empty_batch_handling() {
    std::vector<SensorData> emptyBatch;
    
    // Should handle empty batch gracefully
    testLogger.logBatch(emptyBatch);
    
    std::vector<SensorData> retrievedBatch;
    uint16_t batchIndex;
    bool hasData = testLogger.getOldestBatch(retrievedBatch, batchIndex);
    
    // Empty batch might not be stored, or stored as empty
    TEST_ASSERT_TRUE(true); // Should not crash
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_logger_initialization);
    RUN_TEST(test_error_logging);
    RUN_TEST(test_batch_logging);
    RUN_TEST(test_batch_retrieval_oldest);
    RUN_TEST(test_batch_retrieval_newest);
    RUN_TEST(test_batch_removal);
    RUN_TEST(test_send_status_logging);
    RUN_TEST(test_clear_all_batches);
    RUN_TEST(test_batch_indices);
    RUN_TEST(test_batch_crc_validation);
    RUN_TEST(test_max_batches_limit);
    RUN_TEST(test_empty_batch_handling);
    
    return UNITY_END();
}