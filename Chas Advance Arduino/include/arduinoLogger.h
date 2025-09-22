#ifndef ARDUINOLOGGER_H
#define ARDUINOLOGGER_H

#include <Arduino.h>
#include <EEPROM.h>

// ==== CONFIG ====
#define LOGGER_MAX_ENTRIES   64     // total number of logs in EEPROM
#define LOGGER_MSG_LENGTH    32     // max length of each log message (including '\0')
#define EEPROM_SIZE          (10 + LOGGER_MAX_ENTRIES * LOGGER_MSG_LENGTH)

// ==== LOGGER CLASS ====
class Logger {
public:
    Logger() : head(0), count(0) {}

    void begin();
    void log(const String &msg);
    void printAll();
    String getEntry(size_t index);
    size_t size() { return count; }
    void clearAll();

private:
    char buffer[LOGGER_MAX_ENTRIES][LOGGER_MSG_LENGTH];
    size_t head;
    size_t count;

    void load();
    void saveMeta();
    void saveEntry(size_t index);
};

#endif
