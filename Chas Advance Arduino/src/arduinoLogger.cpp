#include "arduinoLogger.h"
#include <EEPROM.h>
#include "batchHandler.h"

// Initialize the logger
void Logger::begin()
{
    loggerActive = false;
    Serial.println("Logger init for UNO R4 WiFi");

    EEPROM.begin();

    // Load previously stored logs (if any) from EEPROM into RAM buffer
    load();
}

// Add a new log entry
void Logger::log(const String &msg)
{
    Serial.println("Logging on arduino: " + msg);
    // Copy the string safely into the fixed-size buffer for the current head slot
    // Truncates if msg is longer than LOGGER_MSG_LENGTH - 1 to avoid overflow
    msg.substring(0, LOGGER_MSG_LENGTH - 1).toCharArray(buffer[head], LOGGER_MSG_LENGTH);

    // Save the current entry into EEPROM at the corresponding address
    // This rotates through slots in a circular buffer to spread flash wear
    saveEntry(head);

    // Advance the head index (circular buffer logic)
    head = (head + 1) % LOGGER_MAX_ENTRIES;

    // Increase count until maximum number of entries is reached
    if (count < LOGGER_MAX_ENTRIES)
        count++;

    // Persist the circular buffer metadata (head and count) in EEPROM
    saveMeta();
}

// Print all stored logs in order from oldest to newest
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
        return ""; // Return empty string if out of bounds

    // Calculate real index in circular buffer
    size_t realIndex = (head + LOGGER_MAX_ENTRIES - count + index) % LOGGER_MAX_ENTRIES;

    // Return as Arduino String for convenience
    return String(buffer[realIndex]);
}

// Clear all logs from RAM and EEPROM
void Logger::clearAll()
{
    for (size_t i = 0; i < LOGGER_MAX_ENTRIES; i++)
    {
        // Clear the RAM buffer
        memset(buffer[i], 0, LOGGER_MSG_LENGTH);

        // Clear the corresponding EEPROM memory
        // EEPROM.update() only writes if value changes, reducing wear
        for (int j = 0; j < LOGGER_MSG_LENGTH; j++)
        {
            EEPROM.update(10 + i * LOGGER_MSG_LENGTH + j, 0);
        }
    }

    // Reset counters
    head = 0;
    count = 0;

    // Persist metadata to EEPROM
    saveMeta();

    Serial.println("Logs cleared");
}

// Load logs from EEPROM into RAM buffer
void Logger::load()
{
    // Read metadata: total entries and head index
    count = EEPROM.read(0);
    head = EEPROM.read(1);

    // Sanity checks to prevent invalid values (e.g., after flash corruption or first boot)
    if (count > LOGGER_MAX_ENTRIES)
        count = 0;
    if (head >= LOGGER_MAX_ENTRIES)
        head = 0;

    // Load each log entry from EEPROM into RAM buffer
    for (size_t i = 0; i < count; i++)
    {
        for (int j = 0; j < LOGGER_MSG_LENGTH; j++)
        {
            buffer[i][j] = EEPROM.read(10 + i * LOGGER_MSG_LENGTH + j);
        }

        // Ensure null-termination for safety when converting to String
        buffer[i][LOGGER_MSG_LENGTH - 1] = '\0';
    }
}

// Persist metadata (head index and count) in EEPROM
void Logger::saveMeta()
{
    // Using EEPROM.update() to reduce unnecessary flash writes
    EEPROM.update(0, count);
    EEPROM.update(1, head);
}

// Persist a single log entry in EEPROM at the given index
void Logger::saveEntry(size_t index)
{
    int addr = 10 + index * LOGGER_MSG_LENGTH; // Calculate EEPROM offset for this slot

    for (int j = 0; j < LOGGER_MSG_LENGTH; j++)
    {
        // Only write bytes that have changed to reduce flash wear
        EEPROM.update(addr + j, buffer[index][j]);
    }
}

void Logger::update(bool wifiConnected)
{
    std::vector<SensorData> &batch = getBatchBuffer();
    if (batch.empty())
    {
        return; // No data to log
    }

    if (!wifiConnected && !loggerActive)
    {
        // Start logging
        loggerActive = true;
        SensorData medianData = calculateMedian(batch);
        logMedian(medianData);
        timeSinceLog = millis();
    }
    else if (!wifiConnected && loggerActive)
    {
        // Continue logging if 1 minute has passed
        if (millis() - timeSinceLog >= 60000)
        {
            SensorData medianData = calculateMedian(batch);
            logMedian(medianData);
            timeSinceLog = millis();
        }
    }
    else if (wifiConnected && loggerActive)
    {
        // Log once and then stop logging
        SensorData medianData = calculateMedian(batch);
        logMedian(medianData);
        loggerActive = false;
    }

    if (loggerActive)
    {
        for (auto &entry : batch)
        {
            if (entry.error)
            {
                // Log warning if any entry in the batch has an error
                log(String(entry.temperature) + "," +
                    String(entry.humidity) + "," +
                    String(static_cast<int>(entry.errorType)));
                return; // Only log once per batch
            }
        }
    }
}

void Logger::logMedian(const SensorData &medianData)
{
    log(String(medianData.temperature) + "," +
        String(medianData.humidity) +
        (medianData.error ? "1" : "0"));
}