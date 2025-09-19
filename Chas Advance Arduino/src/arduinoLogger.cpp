#include "arduinoLogger.h"
#include <EEPROM.h>

// Constructor: initializes the Logger object
Logger::Logger() : head(0), count(0)
{
    // Initialize the RAM buffer to empty strings
    // This prevents garbage characters when printing before logs are loaded
    for (size_t i = 0; i < LOGGER_MAX_ENTRIES; i++)
        buffer[i][0] = '\0';
}

// Initialize the Logger
void Logger::begin()
{
    Serial.println("Logger init for UNO R4 WiFi");

    // Initialize EEPROM for read/write operations
    // UNO R4 WiFi EEPROM.begin() does not require a size argument
    EEPROM.begin();

    // Optional: clear all old logs in EEPROM (use ONCE during testing or first flash)
    // clearAll();

    // Load existing logs from EEPROM into the RAM buffer
    load();
}

// Add a new log entry
void Logger::log(const String &msg)
{
    // Safely copy the String into the fixed-size char buffer
    // Truncates if msg is longer than LOGGER_MSG_LENGTH - 1
    msg.substring(0, LOGGER_MSG_LENGTH - 1).toCharArray(buffer[head], LOGGER_MSG_LENGTH);

    // Advance the head index (circular buffer)
    head = (head + 1) % LOGGER_MAX_ENTRIES;

    // Increase count until maximum number of entries is reached
    if (count < LOGGER_MAX_ENTRIES)
        count++;

    // Persist only the latest entry to EEPROM to reduce writes
    saveLastEntry();
}

// Print all stored logs to Serial in order from oldest to newest
void Logger::printAll()
{
    Serial.println("---- Log start ----");
    for (size_t i = 0; i < count; i++)
    {
        Serial.println(getEntry(i));
    }
    Serial.println("---- Log end ----");
}

// Retrieve a single log entry by index (0 = oldest)
String Logger::getEntry(size_t index)
{
    if (index >= count)
        return ""; // Return empty string if index is out of bounds

    // Calculate the real index in the circular buffer
    size_t realIndex = (head + LOGGER_MAX_ENTRIES - count + index) % LOGGER_MAX_ENTRIES;

    // Convert the fixed-size char array to Arduino String for easy use
    return String(buffer[realIndex]);
}

// Return the number of log entries currently stored
size_t Logger::size()
{
    return count;
}

// Load logs from EEPROM into RAM buffer
void Logger::load()
{
    // Read stored metadata from EEPROM
    count = EEPROM.read(0);  // number of entries stored
    head = EEPROM.read(1);   // index of the next log entry (head of circular buffer)

    // Sanity checks to prevent invalid values
    if (count > LOGGER_MAX_ENTRIES)
        count = LOGGER_MAX_ENTRIES;
    if (head >= LOGGER_MAX_ENTRIES)
        head = 0;

    // Load each log entry from EEPROM into RAM buffer
    for (size_t i = 0; i < count; i++)
    {
        int addr = 10 + i * LOGGER_MSG_LENGTH; // EEPROM offset for this log

        // Read each character into RAM buffer
        for (int j = 0; j < LOGGER_MSG_LENGTH - 1; j++)
        {
            buffer[i][j] = EEPROM.read(addr + j);
        }

        // Ensure null-termination for safe printing
        buffer[i][LOGGER_MSG_LENGTH - 1] = '\0';
    }
}

// Save only the most recently added log entry to EEPROM
void Logger::saveLastEntry()
{
    // Update metadata in EEPROM
    EEPROM.update(0, count);
    EEPROM.update(1, head);

    // Calculate index of last added entry
    size_t lastIndex = (head + LOGGER_MAX_ENTRIES - 1) % LOGGER_MAX_ENTRIES;
    int addr = 10 + lastIndex * LOGGER_MSG_LENGTH;

    // Write the log entry to EEPROM
    for (int j = 0; j < LOGGER_MSG_LENGTH; j++)
        EEPROM.update(addr + j, buffer[lastIndex][j]);
}

// Clear all logs from RAM and EEPROM
void Logger::clearAll()
{
    for (size_t i = 0; i < LOGGER_MAX_ENTRIES; i++)
    {
        int addr = 10 + i * LOGGER_MSG_LENGTH;

        // Clear the corresponding EEPROM memory
        for (int j = 0; j < LOGGER_MSG_LENGTH; j++)
            EEPROM.update(addr + j, 0);

        // Clear the RAM buffer
        buffer[i][0] = '\0';
    }

    // Reset counters
    count = 0;
    head = 0;

    // Update metadata in EEPROM
    EEPROM.update(0, count);
    EEPROM.update(1, head);
}
