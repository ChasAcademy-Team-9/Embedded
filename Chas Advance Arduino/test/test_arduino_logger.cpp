#include <unity.h>
#include "../include/arduinoLogger.h"
#include "../include/log.h"
#include <EEPROM.h>

Logger testLogger;

void setUp(void)
{
    // Clear EEPROM and reset logger before each test
    testLogger.clearAll();
}

void tearDown(void)
{
    // Clean up after each test
}

// Test logger initialization
void test_logger_initialization()
{
    testLogger.begin();

    TEST_ASSERT_EQUAL_INT(0, testLogger.size());
}

// Test logging single entry
void test_logger_single_entry()
{
    testLogger.begin();

    testLogger.log("23.5,65.2,0");

    TEST_ASSERT_EQUAL_INT(1, testLogger.size());
    TEST_ASSERT_EQUAL_STRING("23.5,65.2,0", testLogger.getEntry(0).c_str());
}

// Test logging multiple entries
void test_logger_multiple_entries()
{
    testLogger.begin();

    testLogger.log("23.5,65.2,0");
    testLogger.log("24.0,66.0,0");
    testLogger.log("22.1,64.5,1");

    TEST_ASSERT_EQUAL_INT(3, testLogger.size());
    TEST_ASSERT_EQUAL_STRING("23.5,65.2,0", testLogger.getEntry(0).c_str());
    TEST_ASSERT_EQUAL_STRING("24.0,66.0,0", testLogger.getEntry(1).c_str());
    TEST_ASSERT_EQUAL_STRING("22.1,64.5,1", testLogger.getEntry(2).c_str());
}

// Test circular buffer behavior (when full)
void test_logger_circular_buffer()
{
    testLogger.begin();

    // Fill buffer beyond capacity
    for (int i = 0; i < LOGGER_MAX_ENTRIES + 5; i++)
    {
        String entry = String(20.0 + i) + ",60.0,0";
        testLogger.log(entry);
    }

    TEST_ASSERT_EQUAL_INT(LOGGER_MAX_ENTRIES, testLogger.size());

    // Check that oldest entries are overwritten
    // First entry should now be entry #5 (since we added 5 extra)
    String expected = String(25.0) + ",60.0,0";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), testLogger.getEntry(0).c_str());
}

// Test clearing all logs
void test_logger_clear_all()
{
    testLogger.begin();

    testLogger.log("23.5,65.2,0");
    testLogger.log("24.0,66.0,0");

    TEST_ASSERT_EQUAL_INT(2, testLogger.size());

    testLogger.clearAll();

    TEST_ASSERT_EQUAL_INT(0, testLogger.size());
}

// Test persistence (EEPROM save/load)
void test_logger_persistence()
{
    // First session - add data
    testLogger.begin();
    testLogger.log("23.5,65.2,0");
    testLogger.log("24.0,66.0,0");

    // Simulate restart - create new logger instance
    Logger newLogger;
    newLogger.begin();

    // Data should be restored from EEPROM
    TEST_ASSERT_EQUAL_INT(2, newLogger.size());
    TEST_ASSERT_EQUAL_STRING("23.5,65.2,0", newLogger.getEntry(0).c_str());
    TEST_ASSERT_EQUAL_STRING("24.0,66.0,0", newLogger.getEntry(1).c_str());
}

// Test logMedian function
void test_log_median()
{
    testLogger.begin();

    SensorData medianData = {1000, 23.5, 65.2, false, NONE};

    testLogger.logMedian(medianData);

    TEST_ASSERT_EQUAL_INT(1, testLogger.size());
    TEST_ASSERT_EQUAL_STRING("23.5,65.2,0", testLogger.getEntry(0).c_str());
}

// Test entry retrieval bounds
void test_logger_entry_bounds()
{
    testLogger.begin();

    testLogger.log("23.5,65.2,0");

    // Valid index
    TEST_ASSERT_NOT_EQUAL("", testLogger.getEntry(0));

    // Invalid index (out of bounds)
    TEST_ASSERT_EQUAL_STRING("", testLogger.getEntry(5).c_str());
    TEST_ASSERT_EQUAL_STRING("", testLogger.getEntry(100).c_str());
}

// Test message truncation (if message too long)
void test_logger_message_truncation()
{
    testLogger.begin();

    // Create very long message (longer than LOGGER_MSG_LENGTH)
    String longMessage = "This_is_a_very_long_message_that_exceeds_the_maximum_allowed_length_for_logging";

    testLogger.log(longMessage);

    String retrieved = testLogger.getEntry(0);

    // Should be truncated to fit LOGGER_MSG_LENGTH - 1 (for null terminator)
    TEST_ASSERT_TRUE(retrieved.length() < longMessage.length());
    TEST_ASSERT_TRUE(retrieved.length() <= LOGGER_MSG_LENGTH - 1);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_logger_initialization);
    RUN_TEST(test_logger_single_entry);
    RUN_TEST(test_logger_multiple_entries);
    RUN_TEST(test_logger_circular_buffer);
    RUN_TEST(test_logger_clear_all);
    RUN_TEST(test_logger_persistence);
    RUN_TEST(test_log_median);
    RUN_TEST(test_logger_entry_bounds);
    RUN_TEST(test_logger_message_truncation);

    return UNITY_END();
}