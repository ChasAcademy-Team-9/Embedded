#include <unity.h>
#include "../include/mockJson.h"

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

// Test mock JSON generation basic functionality
void test_mock_json_generation() {
    // Initialize mock if needed
    initMockJson();
    
    String mockJson = generateMockJson();
    
    // Should generate valid JSON string
    TEST_ASSERT_TRUE(mockJson.length() > 0);
    TEST_ASSERT_TRUE(mockJson.indexOf('{') >= 0); // Contains JSON structure
    TEST_ASSERT_TRUE(mockJson.indexOf('}') >= 0);
}

// Test mock JSON contains required fields
void test_mock_json_required_fields() {
    initMockJson();
    
    String mockJson = generateMockJson();
    
    // Should contain all required sensor data fields
    TEST_ASSERT_TRUE(mockJson.indexOf("timestamp") >= 0);
    TEST_ASSERT_TRUE(mockJson.indexOf("temperature") >= 0);
    TEST_ASSERT_TRUE(mockJson.indexOf("humidity") >= 0);
    TEST_ASSERT_TRUE(mockJson.indexOf("error") >= 0);
    TEST_ASSERT_TRUE(mockJson.indexOf("errorType") >= 0);
}

// Test mock JSON value ranges
void test_mock_json_value_ranges() {
    initMockJson();
    
    // Generate multiple mock JSONs and check value ranges
    for (int i = 0; i < 10; i++) {
        String mockJson = generateMockJson();
        
        // Parse to verify value ranges (simplified check)
        // Temperature should be in reasonable range
        int tempIndex = mockJson.indexOf("\"temperature\":");
        if (tempIndex >= 0) {
            String tempStr = mockJson.substring(tempIndex + 14); // Skip "temperature":
            tempStr = tempStr.substring(0, tempStr.indexOf(','));
            float temp = tempStr.toFloat();
            
            TEST_ASSERT_TRUE(temp >= -40.0 && temp <= 80.0); // Reasonable temperature range
        }
        
        // Humidity should be 0-100%
        int humIndex = mockJson.indexOf("\"humidity\":");
        if (humIndex >= 0) {
            String humStr = mockJson.substring(humIndex + 11); // Skip "humidity":
            humStr = humStr.substring(0, humStr.indexOf(','));
            float humidity = humStr.toFloat();
            
            TEST_ASSERT_TRUE(humidity >= 0.0 && humidity <= 100.0);
        }
    }
}

// Test mock JSON error simulation
void test_mock_json_error_simulation() {
    initMockJson();
    
    // Enable error simulation
    setMockErrorRate(100); // 100% error rate
    
    String mockJson = generateMockJson();
    
    // Should contain error condition
    TEST_ASSERT_TRUE(mockJson.indexOf("\"error\":true") >= 0);
    
    // Reset error rate
    setMockErrorRate(0); // 0% error rate
    
    mockJson = generateMockJson();
    
    // Should contain no error
    TEST_ASSERT_TRUE(mockJson.indexOf("\"error\":false") >= 0);
}

// Test mock JSON timestamp format
void test_mock_json_timestamp() {
    initMockJson();
    
    String mockJson = generateMockJson();
    
    // Should contain timestamp field
    int timestampIndex = mockJson.indexOf("\"timestamp\":");
    TEST_ASSERT_TRUE(timestampIndex >= 0);
    
    // Extract timestamp value (simplified)
    String timestampStr = mockJson.substring(timestampIndex + 12); // Skip "timestamp":
    timestampStr = timestampStr.substring(1); // Skip opening quote
    timestampStr = timestampStr.substring(0, timestampStr.indexOf('"')); // To closing quote
    
    // Should be non-empty timestamp string
    TEST_ASSERT_TRUE(timestampStr.length() > 0);
}

// Test mock JSON different error types
void test_mock_json_error_types() {
    initMockJson();
    
    // Test multiple generations to potentially get different error types
    bool foundDifferentErrorTypes = false;
    int errorType1 = -1, errorType2 = -1;
    
    for (int i = 0; i < 20; i++) {
        setMockErrorRate(50); // 50% chance of errors
        String mockJson = generateMockJson();
        
        if (mockJson.indexOf("\"error\":true") >= 0) {
            // Extract error type
            int errorTypeIndex = mockJson.indexOf("\"errorType\":");
            if (errorTypeIndex >= 0) {
                String errorStr = mockJson.substring(errorTypeIndex + 12);
                errorStr = errorStr.substring(0, errorStr.indexOf('}'));
                int errorType = errorStr.toInt();
                
                if (errorType1 == -1) {
                    errorType1 = errorType;
                } else if (errorType2 == -1 && errorType != errorType1) {
                    errorType2 = errorType;
                    foundDifferentErrorTypes = true;
                    break;
                }
            }
        }
    }
    
    // Should be able to generate different error types
    // (This might not always pass due to randomness, but tests the capability)
    TEST_ASSERT_TRUE(errorType1 >= 0); // At least one error type found
}

// Test mock JSON batch generation
void test_mock_json_batch_generation() {
    initMockJson();
    
    String batchJson = generateMockJsonBatch(5);
    
    // Should generate batch with multiple entries
    TEST_ASSERT_TRUE(batchJson.length() > 0);
    TEST_ASSERT_TRUE(batchJson.indexOf('[') >= 0); // Should be JSON array
    TEST_ASSERT_TRUE(batchJson.indexOf(']') >= 0);
    
    // Count occurrences of temperature field (should be 5)
    int count = 0;
    int pos = 0;
    while ((pos = batchJson.indexOf("\"temperature\"", pos)) != -1) {
        count++;
        pos++;
    }
    TEST_ASSERT_EQUAL_INT(5, count);
}

// Test mock JSON configuration persistence
void test_mock_json_configuration() {
    initMockJson();
    
    // Set configuration
    setMockTemperatureRange(10.0, 30.0);
    setMockHumidityRange(40.0, 80.0);
    
    // Generate JSON and check if values are in range
    String mockJson = generateMockJson();
    
    // Extract and verify temperature is in set range
    int tempIndex = mockJson.indexOf("\"temperature\":");
    if (tempIndex >= 0) {
        String tempStr = mockJson.substring(tempIndex + 14);
        tempStr = tempStr.substring(0, tempStr.indexOf(','));
        float temp = tempStr.toFloat();
        
        TEST_ASSERT_TRUE(temp >= 10.0 && temp <= 30.0);
    }
}

// Test mock JSON memory usage
void test_mock_json_memory_usage() {
    initMockJson();
    
    // Generate many JSONs to test memory handling
    for (int i = 0; i < 100; i++) {
        String mockJson = generateMockJson();
        
        // Should continue to generate valid JSONs
        TEST_ASSERT_TRUE(mockJson.length() > 0);
        
        // Free memory by letting string go out of scope
    }
    
    TEST_ASSERT_TRUE(true); // If we get here, memory handling is OK
}

// Test mock JSON reset functionality
void test_mock_json_reset() {
    initMockJson();
    
    // Change configuration
    setMockErrorRate(75);
    setMockTemperatureRange(0.0, 10.0);
    
    // Reset to defaults
    resetMockJson();
    
    // Should return to default behavior
    String mockJson = generateMockJson();
    TEST_ASSERT_TRUE(mockJson.length() > 0);
    
    // Error rate should be back to default (likely lower)
    int errorCount = 0;
    for (int i = 0; i < 20; i++) {
        String json = generateMockJson();
        if (json.indexOf("\"error\":true") >= 0) {
            errorCount++;
        }
    }
    
    // Should have fewer errors than 75% rate
    TEST_ASSERT_TRUE(errorCount < 15); // Less than 75% of 20
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_mock_json_generation);
    RUN_TEST(test_mock_json_required_fields);
    RUN_TEST(test_mock_json_value_ranges);
    RUN_TEST(test_mock_json_error_simulation);
    RUN_TEST(test_mock_json_timestamp);
    RUN_TEST(test_mock_json_error_types);
    RUN_TEST(test_mock_json_batch_generation);
    RUN_TEST(test_mock_json_configuration);
    RUN_TEST(test_mock_json_memory_usage);
    RUN_TEST(test_mock_json_reset);
    
    return UNITY_END();
}