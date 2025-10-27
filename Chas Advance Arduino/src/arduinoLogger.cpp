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

// Add a new loge ntry
void Logger::log(const LogEntry &entry)
{
   // Store entry in RAM circular buffer
    buffer[head] = entry;

    // Persist to EEPROM at the matching position
    saveEntry(head);

    // Move write pointer forward (wrap around if full)
    head = (head + 1) % LOGGER_MAX_ENTRIES;

    // Increment log count until buffer is full
    if (count < LOGGER_MAX_ENTRIES)
        count++;

    // Update metadata (count + head) in EEPROM
    saveMeta();
}

void Logger::logDataEntry(SensorData &data)
{
    LogEntry entry;
    entry.timestamp = data.timestamp;
    entry.temperature = data.temperature;
    entry.humidity = data.humidity;
    entry.sensorId = data.SensorId;
    entry.errorType = data.errorType;

    log(entry);
    Serial.print("Entry logged: ");
    printEntry(entry);
    Serial.println();
}

// Print all stored logs in order from oldest to newest
void Logger::printAll()
{
    Serial.println("---- Log Start ----");
    for (size_t i = 0; i < count; i++)
    {
        // Compute index in the circular buffer (oldest to newest)
        size_t realIndex = (head + LOGGER_MAX_ENTRIES - count + i) % LOGGER_MAX_ENTRIES;
        LogEntry logEntry = buffer[realIndex];
        Serial.print("[");
        if (i < 10)
        {
            Serial.print("Entry ");
            Serial.print(i);
            Serial.print(": ");
            printEntry(logEntry);
        }
        Serial.println("]");
    }
    Serial.println("---- Log End ----");
}

void Logger::printEntry(LogEntry &entry)
{
    Serial.print("ts=");
    Serial.print(formatUnixTime(entry.timestamp));
    Serial.print(" sensor=");
    Serial.print(entry.sensorId);
    Serial.print(" T=");
    Serial.print(entry.temperature, 2);
    Serial.print("°C H=");
    Serial.print(entry.humidity, 2);
    Serial.print(" Errortype=");
    Serial.print(entry.errorType);
}

// Clear all logs from RAM and EEPROM
void Logger::clearAll()
{
    for (size_t i = 0; i < LOGGER_MAX_ENTRIES; i++)
    {
        // Clear RAM copy
        memset(&buffer[i], 0, sizeof(LogEntry));
         // Clear EEPROM slot byte-by-byte to avoid excessive wear
        int addr = getEepromAddr(i);
        for (size_t j = 0; j < sizeof(LogEntry); j++)
            EEPROM.update(addr + j, 0);
    }

    head = 0;
    count = 0;
    saveMeta();
    Serial.println("Logs cleared");
}

// Load logs from EEPROM into RAM buffer
void Logger::load()
{
count = EEPROM.read(0);
    head  = EEPROM.read(1);

    // Validate metadata values to prevent invalid buffer state
    if (count > LOGGER_MAX_ENTRIES)
        count = 0;
    if (head >= LOGGER_MAX_ENTRIES)
        head = 0;

    // Load all stored log entries into RAM buffer
    for (size_t i = 0; i < count; i++)
        buffer[i] = readEntry(i);

    Serial.print("Loaded " + String(count) + " log entries (head=" + String(head) + ")\n");
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
    int addr = getEepromAddr(index);
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&buffer[index]);
    for (size_t i = 0; i < sizeof(LogEntry); i++)
        EEPROM.update(addr + i, ptr[i]);
}

// Read one log entry (binary) from EEPROM and return it as a LogEntry struct.
LogEntry Logger::readEntry(size_t index)
{
    LogEntry entry;
    int addr = getEepromAddr(index);
    // Copy raw bytes from EEPROM into struct memory
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&entry);
    for (size_t i = 0; i < sizeof(LogEntry); i++)
        ptr[i] = EEPROM.read(addr + i);

    return entry;
}

// Calculate EEPROM address for a given log entry index.
// Takes into account metadata space at the beginning of EEPROM.
size_t Logger::getEepromAddr(size_t index)
{
    return EEPROM_META_SIZE + index * sizeof(LogEntry);
}

void Logger::update(bool wifiConnected)
{
    std::vector<SensorData> &batch = getBatchBuffer();
    if (batch.empty())
        return;

    unsigned long now = millis();
    bool logNow = false;

    // 1. Wi-Fi just disconnected -> log immediately
    if (!wifiConnected && !loggerActive)
    {
        loggerActive = true;
        logNow = true;
    }
    // 2. Wi-Fi disconnected, continue logging every minute
    else if (!wifiConnected && loggerActive && (now - timeSinceLog >= 60000))
    {
        logNow = true;
    }
    // 3. Wi-Fi reconnected -> log once and stop logging
    else if (wifiConnected && loggerActive)
    {
        logNow = true;
        loggerActive = false;
    }
    if (logNow)
        createLogFromBatch(batch, now);
}

void Logger::createLogFromBatch(std::vector<SensorData> &batch, unsigned long now)
{
    bool errorLogged = false;
    for (auto &entry : batch)
    {
        if (entry.error)
        {
            logDataEntry(entry);
            errorLogged = true;
            break;
        }
    }

    if (!errorLogged)
    {
        // No errors, log median
        SensorData medianData = calculateMedian(batch);
        logMedian(medianData);
    }

    timeSinceLog = now; // Reset timer
}

void Logger::logMedian(SensorData &medianData)
{
    if (medianData.error)
        return;

    logDataEntry(medianData);
}