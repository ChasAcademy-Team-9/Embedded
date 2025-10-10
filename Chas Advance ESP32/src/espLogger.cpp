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

void ESPLogger::logBatch(std::vector<SensorData>& entries)
   {
    if (entries.empty())
    {
        Serial.println("No valid entries in JSON array; batch not logged");
        return;
    }

    // Calculate CRC for integrity check
    uint32_t crc = CRC32::calculate((uint8_t *)entries.data(), entries.size() * sizeof(SensorData));
    
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
    size_t written = 0; //Variable to keep track of bytes written
    written += f.write((uint8_t *)&count, sizeof(count));
    written += f.write((uint8_t *)entries.data(), count * sizeof(SensorData));
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
        std::vector<SensorData> entries;
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

bool ESPLogger::getOldestBatch(std::vector<SensorData> &outEntries, uint16_t &batchIndex)
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

bool ESPLogger::getNewestBatch(std::vector<SensorData> &outEntries, uint16_t &batchIndex)
{
    auto indices = getBatchIndices();
    if (indices.empty())
        return false;

    batchIndex = indices.back(); // newest batch
    String fileName = getBatchFilename(batchIndex);

    if (!readBatchFile(fileName, outEntries))
    {
        Serial.printf("Batch %s is corrupted, removing file\n", fileName.c_str());
        LittleFS.remove(fileName);
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

void ESPLogger::logSendStatus(int batchId, bool success, const String &message)
{
    if (batchId < 0) return;

    const size_t MAX_LOG_SIZE = 10 * 1024; // 10 KB max
    const char *filename = "/send_status.bin";

    // Check log size
    if (LittleFS.exists(filename)) {
        File logFile = LittleFS.open(filename, FILE_READ);
        if (logFile && logFile.size() > MAX_LOG_SIZE) {
            logFile.close();
            Serial.println("Send status log too large, clearing...");
            LittleFS.remove(filename);
        } else {
            logFile.close();
        }
    }

    File sendStatusLogFile = LittleFS.open(filename, FILE_APPEND);
    if (!sendStatusLogFile) {
        Serial.println("Failed to open send status log");
        return;
    }

    SendStatusEntry entry;
    entry.timestamp = timestampStringToUnix(getTimeStamp());
    entry.batchId = batchId;
    entry.success = success;

    // Copy message safely (truncate if needed)
    strncpy(entry.message, message.c_str(), sizeof(entry.message) - 1);
    entry.message[sizeof(entry.message) - 1] = '\0';

    // Write binary struct
    sendStatusLogFile.write(reinterpret_cast<const uint8_t*>(&entry), sizeof(entry));
    sendStatusLogFile.close();
}

void ESPLogger::printSendStatusLogs()
{
    const char *filename = "/send_status.bin";
    File logFile = LittleFS.open(filename, FILE_READ);
    if (!logFile) {
        Serial.println("No send status log found");
        return;
    }

    Serial.println("---- Send Status Log Start ----");
    SendStatusEntry entry;

    while (logFile.available() >= sizeof(SendStatusEntry)) {
        logFile.read(reinterpret_cast<uint8_t*>(&entry), sizeof(entry));
        Serial.printf("Time:%s | Batch:%d | Status:%s | %s\n",
                      formatUnixTime(entry.timestamp).c_str(),
                      entry.batchId,
                      entry.success ? "OK" : "FAIL",
                      entry.message);
    }

    Serial.println("---- Send Status Log End ----");
    logFile.close();
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
            uint16_t idx = name.substring(7, name.length() - 4).toInt(); // Extract index out of name
            indices.push_back(idx);
        }
        file = root.openNextFile();
    }
    std::sort(indices.begin(), indices.end()); // Sort indices in ascending order
    return indices;
}

bool ESPLogger::readBatchFile(const String &fname, std::vector<SensorData> &outEntries)
{
    File f = LittleFS.open(fname, FILE_READ);
    if (!f)
        return false;

    outEntries.clear();

    if (f.size() < sizeof(uint16_t) + sizeof(uint32_t))
    {
        f.close(); // File too small to contain count + CRC
        return false;
    }

    uint16_t count = 0; // Read number of entries (count) - Must be >0 and reasonable.
    if (f.read((uint8_t *)&count, sizeof(count)) != sizeof(count) || count == 0 || count > 100)
    {
        f.close();
        return false;
    }
    // Check file size matches expected size (count + entries + CRC)
    if (f.size() < sizeof(uint16_t) + count * sizeof(SensorData) + sizeof(uint32_t))
    {
        f.close();
        return false;
    }

    outEntries.resize(count); // Allocate space for entries and then read entries into vector
    if (f.read((uint8_t *)outEntries.data(), count * sizeof(SensorData)) != count * sizeof(SensorData))
    { 
        f.close();
        return false;
    }

    uint32_t savedCrc = 0; // Read stored CRC, pointer is now at end of file
    if (f.read((uint8_t *)&savedCrc, sizeof(savedCrc)) != sizeof(savedCrc))
    {
        f.close();
        return false;
    }

    f.close();

    uint32_t calcCrc = CRC32::calculate((uint8_t *)outEntries.data(), count * sizeof(SensorData));
    if (calcCrc != savedCrc)
        return false;

    return true;
}

void ESPLogger::printEntries(const std::vector<SensorData> &entries)
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
