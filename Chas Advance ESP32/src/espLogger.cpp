#include "espLogger.h"

ESPLogger::ESPLogger() {}

void ESPLogger::begin()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed");
    }
    else
    {
        Serial.println("LittleFS mounted successfully");
    }
}

// -------- Error logging --------
void ESPLogger::logError(const String &msg)
{
    File f = LittleFS.open(ERROR_FILE, FILE_APPEND);
    if (!f)
    {
        Serial.println("Failed to open error log");
        return;
    }

    String timeStamp = getTimeStamp();
    f.printf("%s | %s\n", timeStamp.c_str(), msg.c_str());
    f.close();
}

void ESPLogger::printErrors()
{
    File f = LittleFS.open(ERROR_FILE, FILE_READ);
    if (!f)
    {
        Serial.println("No Error log found");
        return;
    }
    Serial.println("---- Error log start ----");
    while (f.available())
    {
        Serial.println(f.readStringUntil('\n'));
    }
    Serial.println("---- Error log end ----");
    f.close();
}

void ESPLogger::clearErrors()
{
    LittleFS.remove(ERROR_FILE);
}

void ESPLogger::logBatch(JsonArray arr)
{
    std::vector<SensorEntry> entries;

    // Convert JSON objects to SensorEntry structs
    for (JsonObject obj : arr)
    {
        SensorEntry entry;
        entry.timestamp = timestampStringToUnix(getTimeStamp());
        entry.temperature = obj["temperature"] | 0.0f;
        entry.humidity = obj["humidity"] | 0.0f;
        entry.error = obj["error"] | false;
        entry.errorType = obj["errorType"] | 0;
        entries.push_back(entry);
    }

    if (entries.empty())
    {
        Serial.println("No valid entries in JSON array; batch not logged");
        return;
    }

    // Calculate CRC for integrity check
    uint32_t crc = CRC32::calculate((uint8_t *)entries.data(), entries.size() * sizeof(SensorEntry));
    
    //crc = 9999; // For testing, force bad CRC

    // Determine next batch index
    auto indices = getBatchIndices();
    uint16_t nextIndex = indices.empty() ? 0 : indices.back() + 1;

    // Keep batch count below MAX_BATCHES
    if (indices.size() >= MAX_BATCHES)
    {
        Serial.println("Max batch files reached; removing oldest batch");
        removeOldestBatch();
    }

    String fname = getBatchFilename(nextIndex);
    Serial.printf("Creating batch file: %s\n", fname.c_str());

    File f = LittleFS.open(fname, FILE_WRITE);
    if (!f)
    {
        Serial.printf("Failed to open batch file %s for writing\n", fname.c_str());
        logError("Failed to create batch file");
        return;
    }

    uint16_t count = entries.size();
    size_t written = 0;
    written += f.write((uint8_t *)&count, sizeof(count));
    written += f.write((uint8_t *)entries.data(), count * sizeof(SensorEntry));
    written += f.write((uint8_t *)&crc, sizeof(crc));
    f.close();

    Serial.printf("Logged batch %d with %d entries, total bytes written: %d\n",
                  nextIndex, count, (int)written);
}

void ESPLogger::printBatches()
{
    auto indices = getBatchIndices();
    if (indices.empty())
    {
        Serial.println("No batch log found");
        return;
    }

    Serial.println("---- Batch log start ----");

    for (uint16_t idx : indices)
    {
        String fname = getBatchFilename(idx);
        std::vector<SensorEntry> entries;
        // Read and validate batch file
        if (!readBatchFile(fname, entries))
        {
            Serial.printf("Corrupt batch %s skipped\n", fname.c_str());
            continue;
        }
        Serial.printf("Batch file: %s\n", fname.c_str());
        printEntries(entries);
        Serial.println("-- End of batch --");
    }

    Serial.println("---- Batch log end ----");
}

bool ESPLogger::getOldestBatch(std::vector<SensorEntry> &outEntries, uint16_t &batchIndex)
{
    auto indices = getBatchIndices();
    if (indices.empty())
        return false;

    batchIndex = indices.front(); // oldest batch
    String fname = getBatchFilename(batchIndex);

    if (!readBatchFile(fname, outEntries))
    {
        Serial.printf("Batch %s is corrupted, removing file\n", fname.c_str());
        LittleFS.remove(fname);
        return false;
    }

    return true;
}

void ESPLogger::removeOldestBatch()
{
    auto indices = getBatchIndices();
    if (indices.empty())
        return;

    String fname = getBatchFilename(indices.front());
    Serial.printf("Removing oldest batch: %s\n", fname.c_str());
    LittleFS.remove(fname);
}

void ESPLogger::clearBatches()
{
    auto indices = getBatchIndices();
    for (uint16_t idx : indices)
    {
        String fname = getBatchFilename(idx);
        Serial.printf("Removing batch file: %s\n", fname.c_str());
        LittleFS.remove(fname);
    }
}

// -------- Batch file utilities --------
String ESPLogger::getBatchFilename(uint16_t index)
{
    return "/batch_" + String(index) + ".bin";
}

std::vector<uint16_t> ESPLogger::getBatchIndices()
{
    std::vector<uint16_t> indices;
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file)
    {
        String name = file.name();
        if (!name.startsWith("/"))
            name = "/" + name;

        if (name.startsWith("/batch_") && name.endsWith(".bin"))
        {
            uint16_t idx = name.substring(7, name.length() - 4).toInt();
            indices.push_back(idx);
            Serial.printf("Found batch file: %s, index: %d\n", name.c_str(), idx);
        }
        file = root.openNextFile();
    }
    std::sort(indices.begin(), indices.end());
    return indices;
}

bool ESPLogger::readBatchFile(const String &fname, std::vector<SensorEntry> &outEntries)
{
    File f = LittleFS.open(fname, FILE_READ);
    if (!f)
        return false;

    outEntries.clear();

    // File too small to contain count + CRC
    if (f.size() < sizeof(uint16_t) + sizeof(uint32_t))
    {
        f.close();
        return false;
    }

    uint16_t count = 0;
    if (f.read((uint8_t *)&count, sizeof(count)) != sizeof(count) || count == 0 || count > 100)
    {
        f.close();
        return false;
    }

    if (f.size() < sizeof(uint16_t) + count * sizeof(SensorEntry) + sizeof(uint32_t))
    {
        f.close();
        return false;
    }

    outEntries.resize(count);
    if (f.read((uint8_t *)outEntries.data(), count * sizeof(SensorEntry)) != count * sizeof(SensorEntry))
    {
        f.close();
        return false;
    }

    uint32_t savedCrc = 0;
    if (f.read((uint8_t *)&savedCrc, sizeof(savedCrc)) != sizeof(savedCrc))
    {
        f.close();
        return false;
    }

    f.close();

    uint32_t calcCrc = CRC32::calculate((uint8_t *)outEntries.data(), count * sizeof(SensorEntry));
    if (calcCrc != savedCrc)
        return false;

    return true;
}

uint32_t ESPLogger::timestampStringToUnix(const String &tsStr)
{
    struct tm timeinfo = {0};

    if (sscanf(tsStr.c_str(), "%4d-%2d-%2d %2d:%2d:%2d",
               &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday,
               &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec) != 6)
    {
        Serial.println("Failed to parse timestamp");
        return 0;
    }

    timeinfo.tm_year -= 1900; // struct tm expects years since 1900
    timeinfo.tm_mon -= 1;     // struct tm months are 0-11

    return (uint32_t)mktime(&timeinfo);
}

void ESPLogger::printEntries(const std::vector<SensorEntry> &entries)
{
    for (const auto &entry : entries)
    {
        Serial.printf("Timestamp: %s, Temp: %.2f, Hum: %.2f, Error: %d, ErrorType: %d\n",
                      formatUnixTime(entry.timestamp).c_str(),
                      entry.temperature,
                      entry.humidity,
                      entry.error,
                      entry.errorType);
    }
}
