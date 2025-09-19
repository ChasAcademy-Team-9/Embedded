#include "arduinoLogger.h"
#include <EEPROM.h>

Logger::Logger() : head(0), count(0) {}

void Logger::begin() {
    Serial.println("Logger init for UNO R4 WiFi");
    EEPROM.begin(); // UNO R4 doesn’t take an argument
    load();
}

void Logger::log(const String &msg) {
    // Copy safely into fixed-size char buffer
    msg.substring(0, LOGGER_MSG_LENGTH - 1).toCharArray(buffer[head], LOGGER_MSG_LENGTH);

    // Advance head
    head = (head + 1) % LOGGER_MAX_ENTRIES;
    if (count < LOGGER_MAX_ENTRIES) count++;

    saveLastEntry(); // only save what changed
}

void Logger::printAll() {
    Serial.println("---- Log start ----");
    for (size_t i = 0; i < count; i++) {
        Serial.println(getEntry(i));
    }
    Serial.println("---- Log end ----");
}

String Logger::getEntry(size_t index) {
    if (index >= count) return "";
    size_t realIndex = (head + LOGGER_MAX_ENTRIES - count + index) % LOGGER_MAX_ENTRIES;
    return String(buffer[realIndex]);
}

size_t Logger::size() {
    return count;
}

void Logger::load() {
    count = EEPROM.read(0);
    if (count > LOGGER_MAX_ENTRIES) count = LOGGER_MAX_ENTRIES;
    head  = EEPROM.read(1);

    for (size_t i = 0; i < count; i++) {
        int addr = 10 + i * LOGGER_MSG_LENGTH;
        for (int j = 0; j < LOGGER_MSG_LENGTH; j++) {
            buffer[i][j] = EEPROM.read(addr + j);
        }
    }
}

void Logger::saveLastEntry() {

    EEPROM.update(0, count);
    EEPROM.update(1, head);

    size_t lastIndex = (head + LOGGER_MAX_ENTRIES - 1) % LOGGER_MAX_ENTRIES;
    int addr = 10 + lastIndex * LOGGER_MSG_LENGTH;
    for (int j = 0; j < LOGGER_MSG_LENGTH; j++) {
        EEPROM.update(addr + j, buffer[lastIndex][j]);
    }
}

void Logger::clearAll() {

    for (size_t i = 0; i < count; i++) {
        int addr = 10 + i * LOGGER_MSG_LENGTH;
        for (int j = 0; j < LOGGER_MSG_LENGTH; j++) {
            EEPROM.update(addr + j, 0);
        }
        memset(buffer[i], 0, LOGGER_MSG_LENGTH);
    }
    count = 0;
    head = 0;
    EEPROM.update(0, count);
    EEPROM.update(1, head);
}
