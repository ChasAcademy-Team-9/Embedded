#ifndef ARDUINOLOGGER_H
#define ARDUINOLOGGER_H

#include <Arduino.h>
#include <EEPROM.h>
#include "sensorData.h"
#include <vector>

// ==== CONFIG ====
#define LOGGER_MAX_ENTRIES 64 // total number of logs in EEPROM
#define EEPROM_META_SIZE 10   // reserved bytes for metadata
#define EEPROM_SIZE (EEPROM_META_SIZE + LOGGER_MAX_ENTRIES * sizeof(LogEntry))

struct LogEntry
{
    uint32_t timestamp;
    float temperature;
    float humidity;
    uint8_t sensorId;
    uint8_t errorType;
};


// ==== LOGGER CLASS ====
class Logger
{
public:
    Logger() : head(0), count(0), loggerActive(false), timeSinceLog(0) {}

    void begin();
    void log(const LogEntry &entry);
    void logDataEntry(SensorData &data);
    void printAll();
    void printEntry(LogEntry &entry);
    size_t size() { return count; }
    void clearAll();
    void update(bool wifiConnected);
    void logMedian(SensorData &medianData);
    void createLogFromBatch(std::vector<SensorData> &batch, unsigned long now);    

    bool loggerActive;

private:
    LogEntry buffer[LOGGER_MAX_ENTRIES];
    size_t head;
    size_t count;
    unsigned long timeSinceLog;

    void load();
    void saveMeta();
    void saveEntry(size_t index);
    LogEntry readEntry(size_t index);
    size_t getEepromAddr(size_t index);
};

#endif
