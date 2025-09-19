#ifndef ESPLOGGER_H
#define ESPLOGGER_H

#include <Arduino.h>

// Max number of log entries
#define LOGGER_MAX_ENTRIES 20
// Max length of each log message
#define LOGGER_MSG_LENGTH 100

class Logger {
public:
    Logger();
    void begin();

    // Add a log entry
    void log(const String &msg);

    // Print all log entries to Serial
    void printAll();

    // Get a specific log entry by index
    String getEntry(size_t index);

    // Get number of stored log entries
    size_t size();
    

private:
    void load();
    void saveLastEntry();
    void clearAll();

    char buffer[LOGGER_MAX_ENTRIES][LOGGER_MSG_LENGTH];
    size_t head;   // index of the next write position
    size_t count;  // number of valid entries
};

#endif
