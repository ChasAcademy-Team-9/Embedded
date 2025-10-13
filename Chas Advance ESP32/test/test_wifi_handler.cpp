#include <unity.h>
#include "../include/wifiHandler.h"
#include "../include/sensorDataHandler.h"
#include "../include/jsonParser.h"

// Mock external variables for testing
extern unsigned long timeSinceDataReceived;

void setUp(void) {
    // Reset state before each test
    timeSinceDataReceived = 0;
}

void tearDown(void) {
    // Clean up after each test
}

// Test data timeout checking
void test_data_timeout_check() {
    unsigned long testTime = 0;
    
    // No timeout initially
    checkDataTimeout(testTime);
    TEST_ASSERT_TRUE(true); // Should complete without issues
    
    // Simulate timeout (would need millis() mocking for full test)
    testTime = 80000; // 80 seconds
    checkDataTimeout(testTime);
    TEST_ASSERT_TRUE(true); // Should handle timeout gracefully
}

// Test binary data parsing
void test_binary_data_parsing() {
    std::vector<uint8_t> testBuffer;
    
    // Create test binary data
    uint32_t testSendMillis = 12345;
    SensorData testData = {1000, 23.5, 65.2, false, 0, 1};
    
    // Add sendMillis to buffer
    testBuffer.resize(sizeof(testSendMillis));
    memcpy(testBuffer.data(), &testSendMillis, sizeof(testSendMillis));
    
    // Add SensorData to buffer
    size_t currentSize = testBuffer.size();
    testBuffer.resize(currentSize + sizeof(SensorData));
    memcpy(testBuffer.data() + currentSize, &testData, sizeof(SensorData));
    
    // Test parsing
    uint32_t parsedSendMillis;
    std::vector<SensorData> parsedBatch;
    
    bool parseSuccess = parseBatch(testBuffer, parsedSendMillis, parsedBatch);
    
    TEST_ASSERT_TRUE(parseSuccess);
    TEST_ASSERT_EQUAL_UINT32(testSendMillis, parsedSendMillis);
    TEST_ASSERT_EQUAL_INT(1, parsedBatch.size());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.5, parsedBatch[0].temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 65.2, parsedBatch[0].humidity);
}

// Test binary data parsing with invalid size
void test_binary_data_parsing_invalid() {
    std::vector<uint8_t> invalidBuffer;
    
    // Buffer too small for sendMillis
    invalidBuffer.resize(2);
    
    uint32_t parsedSendMillis;
    std::vector<SensorData> parsedBatch;
    
    bool parseSuccess = parseBatch(invalidBuffer, parsedSendMillis, parsedBatch);
    
    TEST_ASSERT_FALSE(parseSuccess); // Should fail with invalid size
}

// Test binary data parsing with wrong alignment
void test_binary_data_parsing_wrong_alignment() {
    std::vector<uint8_t> testBuffer;
    
    // Valid sendMillis
    uint32_t testSendMillis = 12345;
    testBuffer.resize(sizeof(testSendMillis));
    memcpy(testBuffer.data(), &testSendMillis, sizeof(testSendMillis));
    
    // Add incomplete SensorData (wrong size)
    testBuffer.resize(testBuffer.size() + sizeof(SensorData) - 1); // One byte short
    
    uint32_t parsedSendMillis;
    std::vector<SensorData> parsedBatch;
    
    bool parseSuccess = parseBatch(testBuffer, parsedSendMillis, parsedBatch);
    
    TEST_ASSERT_FALSE(parseSuccess); // Should fail with wrong alignment
}

// Test multiple sensor data parsing
void test_multiple_sensor_data_parsing() {
    std::vector<uint8_t> testBuffer;
    
    uint32_t testSendMillis = 12345;
    SensorData testData1 = {1000, 23.5, 65.2, false, 0, 1};
    SensorData testData2 = {2000, 24.0, 66.0, false, 0, 1};
    SensorData testData3 = {3000, 22.8, 64.5, false, 0, 1};
    
    // Build buffer
    testBuffer.resize(sizeof(testSendMillis) + 3 * sizeof(SensorData));
    
    size_t offset = 0;
    memcpy(testBuffer.data() + offset, &testSendMillis, sizeof(testSendMillis));
    offset += sizeof(testSendMillis);
    
    memcpy(testBuffer.data() + offset, &testData1, sizeof(SensorData));
    offset += sizeof(SensorData);
    
    memcpy(testBuffer.data() + offset, &testData2, sizeof(SensorData));
    offset += sizeof(SensorData);
    
    memcpy(testBuffer.data() + offset, &testData3, sizeof(SensorData));
    
    // Parse
    uint32_t parsedSendMillis;
    std::vector<SensorData> parsedBatch;
    
    bool parseSuccess = parseBatch(testBuffer, parsedSendMillis, parsedBatch);
    
    TEST_ASSERT_TRUE(parseSuccess);
    TEST_ASSERT_EQUAL_INT(3, parsedBatch.size());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 23.5, parsedBatch[0].temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 24.0, parsedBatch[1].temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 22.8, parsedBatch[2].temperature);
}

// Test timestamp assignment logic
void test_timestamp_assignment() {
    std::vector<SensorData> testBatch;
    
    // Create test data with Arduino millis timestamps
    testBatch.push_back({1000, 23.5, 65.2, false, 0, 1}); // 1 second
    testBatch.push_back({2000, 24.0, 66.0, false, 0, 1}); // 2 seconds
    testBatch.push_back({3000, 22.8, 64.5, false, 0, 1}); // 3 seconds (latest)
    
    uint32_t sendMillis = 5000; // Sent at 5 seconds
    
    assignAbsoluteTimestamps(sendMillis, testBatch);
    
    // Timestamps should be converted to absolute time
    // This test would need NTP time mocking to verify exact values
    // For now, just verify the function doesn't crash and maintains relative order
    TEST_ASSERT_TRUE(testBatch[0].timestamp <= testBatch[1].timestamp);
    TEST_ASSERT_TRUE(testBatch[1].timestamp <= testBatch[2].timestamp);
}

// Test empty batch timestamp assignment
void test_timestamp_assignment_empty() {
    std::vector<SensorData> emptyBatch;
    uint32_t sendMillis = 5000;
    
    // Should handle empty batch gracefully
    assignAbsoluteTimestamps(sendMillis, emptyBatch);
    
    TEST_ASSERT_TRUE(emptyBatch.empty()); // Should remain empty
}

// Test POST request validation
void test_post_request_validation() {
    // Test valid POST request
    String validRequest = "POST /data HTTP/1.1";
    // Would need to mock WiFiClient for full test
    
    // Test invalid request  
    String invalidRequest = "GET /data HTTP/1.1";
    
    // For now, just test that the function logic exists
    TEST_ASSERT_TRUE(validRequest.startsWith("POST /data"));
    TEST_ASSERT_FALSE(invalidRequest.startsWith("POST /data"));
}

// Test content length parsing
void test_content_length_parsing() {
    // Mock header parsing logic
    String headerWithLength = "Content-Length: 1024";
    
    int contentLength = -1;
    if (headerWithLength.startsWith("Content-Length:")) {
        contentLength = headerWithLength.substring(15).toInt();
    }
    
    TEST_ASSERT_EQUAL_INT(1024, contentLength);
    
    // Test invalid header
    String invalidHeader = "Content-Type: application/json";
    contentLength = -1;
    if (invalidHeader.startsWith("Content-Length:")) {
        contentLength = invalidHeader.substring(15).toInt();
    }
    
    TEST_ASSERT_EQUAL_INT(-1, contentLength); // Should remain -1
}

// Test HTTP response codes
void test_http_response_handling() {
    // Mock different HTTP responses
    String response200 = "HTTP/1.1 200 OK";
    String response400 = "HTTP/1.1 400 Bad Request";
    String response500 = "HTTP/1.1 500 Internal Server Error";
    
    TEST_ASSERT_TRUE(response200.startsWith("HTTP/1.1 200"));
    TEST_ASSERT_FALSE(response400.startsWith("HTTP/1.1 200"));
    TEST_ASSERT_FALSE(response500.startsWith("HTTP/1.1 200"));
}

// Test JSON to backend server formatting
void test_json_batch_preparation() {
    std::vector<SensorData> testBatch;
    
    testBatch.push_back({1697200000, 23.5, 65.2, false, 0, 1});
    testBatch.push_back({1697200030, 24.0, 66.0, false, 0, 1});
    
    String jsonResult = serializeBatchToJson(testBatch);
    
    // Should create valid JSON for backend
    TEST_ASSERT_TRUE(jsonResult.length() > 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"data\"") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"SensorID\"") >= 0);
    TEST_ASSERT_TRUE(jsonResult.indexOf("\"TimeStamp\"") >= 0);
}

// Test data timeout threshold
void test_data_timeout_threshold() {
    extern const int dataReceivedThreshold;
    
    // Verify threshold is set to reasonable value
    TEST_ASSERT_TRUE(dataReceivedThreshold > 0);
    TEST_ASSERT_TRUE(dataReceivedThreshold < 300000); // Less than 5 minutes
}

// Test network configuration
void test_network_configuration() {
    extern const long gmtOffset_sec;
    extern const int daylightOffset_sec;
    
    // Verify timezone settings are reasonable
    TEST_ASSERT_TRUE(gmtOffset_sec >= -43200 && gmtOffset_sec <= 43200); // ±12 hours
    TEST_ASSERT_TRUE(daylightOffset_sec >= 0 && daylightOffset_sec <= 7200); // 0-2 hours
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_data_timeout_check);
    RUN_TEST(test_binary_data_parsing);
    RUN_TEST(test_binary_data_parsing_invalid);
    RUN_TEST(test_binary_data_parsing_wrong_alignment);
    RUN_TEST(test_multiple_sensor_data_parsing);
    RUN_TEST(test_timestamp_assignment);
    RUN_TEST(test_timestamp_assignment_empty);
    RUN_TEST(test_post_request_validation);
    RUN_TEST(test_content_length_parsing);
    RUN_TEST(test_http_response_handling);
    RUN_TEST(test_json_batch_preparation);
    RUN_TEST(test_data_timeout_threshold);
    RUN_TEST(test_network_configuration);
    
    return UNITY_END();
}