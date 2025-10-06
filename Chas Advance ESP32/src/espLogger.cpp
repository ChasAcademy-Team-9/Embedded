#include "espLogger.h"
#include <Preferences.h>
#include "sensorDataHandler.h"
#include "log.h"

// Create a global Preferences object for ESP32 non-volatile storage
Preferences prefs;

// Constructor initializes internal buffer counters
Logger::Logger() : head(0), count(0) {}

// Initialize the logger
void Logger::begin()
{
    Serial.println("Logger init for ESP32");

    loggerActive = false;
    // Open the "logger" namespace in Preferences
    // false = read/write mode
    prefs.begin("logger", false);

    // Optional: clear all logs in NVS for testing
    // prefs.clear();

    // Load existing logs from flash into RAM buffer
    load();
}

// Add a new log entry
void Logger::log(const String &msg)
{
    Serial.println("Logging on ESP32: " + msg);
    // Copy the String message into the fixed-size char buffer
    // Truncate if longer than LOGGER_MSG_LENGTH - 1
    msg.substring(0, LOGGER_MSG_LENGTH - 1).toCharArray(buffer[head], LOGGER_MSG_LENGTH);

    // Advance the head pointer (circular buffer)
    head = (head + 1) % LOGGER_MAX_ENTRIES;

    // Increase count until it reaches max entries
    if (count < LOGGER_MAX_ENTRIES)
        count++;

    // Persist only the latest entry and update metadata
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
        return ""; // Return empty if index is out of bounds

    // Calculate the real index in the circular buffer
    size_t realIndex = (head + LOGGER_MAX_ENTRIES - count + index) % LOGGER_MAX_ENTRIES;

    // Convert char array to String for ease of use
    return String(buffer[realIndex]);
}

// Return the number of log entries currently stored
size_t Logger::size()
{
    return count;
}

// Load logs from non-volatile storage into RAM buffer
void Logger::load()
{
    // Read stored count and head from Preferences
    count = prefs.getUInt("count", 0);
    if (count > LOGGER_MAX_ENTRIES)
        count = LOGGER_MAX_ENTRIES; // Ensure we don't exceed buffer

    head = prefs.getUInt("head", 0);

    // Load each stored log into RAM buffer
    for (size_t i = 0; i < count; i++)
    {
        String s = prefs.getString(("log" + String(i)).c_str(), ""); // default to empty
        s.toCharArray(buffer[i], LOGGER_MSG_LENGTH);                 // store as fixed-size char array
    }
}

// Persist only the most recently added entry to Preferences
void Logger::saveLastEntry()
{
    // Save metadata: total count and head pointer
    prefs.putUInt("count", count);
    prefs.putUInt("head", head);

    // Calculate index of the last added log (circular buffer)
    size_t lastIndex = (head + LOGGER_MAX_ENTRIES - 1) % LOGGER_MAX_ENTRIES;

    // Save only the latest log entry to NVS
    prefs.putString(("log" + String(lastIndex)).c_str(), buffer[lastIndex]);
}

// Clear all logs from RAM and non-volatile storage
void Logger::clearAll()
{
    for (size_t i = 0; i < count; i++)
    {
        // Remove each log key from Preferences
        prefs.remove(("log" + String(i)).c_str());

        // Clear the RAM buffer
        memset(buffer[i], 0, LOGGER_MSG_LENGTH);
    }

    // Reset counters
    count = 0;
    head = 0;

    // Update metadata in Preferences
    prefs.putUInt("count", count);
    prefs.putUInt("head", head);
}

void Logger::update(bool Connected, JsonArray arr)
{
    // Compute median of the entire array
    SensorData medianLog = calcMedian(arr);
    String timeStamp = getTimeStamp();

    if (!Connected && !loggerActive)
    {
        log("Server disconnected, starting logger");
        loggerActive = true;
    }
    else if (!Connected && loggerActive)
    {
        String logEntry = timeStamp + " Temp: " + String(medianLog.temperature, 2) + " C, Humidity: " + String(medianLog.humidity, 2) + " %";
        log(logEntry);
    }
    else if (Connected && loggerActive)
    {
        log("Server connected, stopping logger");
        loggerActive = false;
    }

    if (loggerActive)
    {
        for (JsonObject obj : arr)
        {
            int errorTypeInt = obj["errorType"] | 0;
            ErrorType errorType = static_cast<ErrorType>(errorTypeInt);
            bool error = obj["error"] | false;
            if (error)
            {
                String logEntry = timeStamp + " T: " +
                                  String(medianLog.temperature, 2) + " C, H: " +
                                  String(medianLog.humidity, 2) + " % Err: " + String(errorTypeInt);
                log(logEntry);
                break; // Log only once per batch if any error is found
            }
        }
    }
}
