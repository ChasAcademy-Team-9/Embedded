#ifndef ESPLOGGER_H
#define ESPLOGGER_H

#include "FS.h"
#include "LittleFS.h"
#include <ArduinoJson.h>
#include "sensorDataHandler.h"
#include "log.h"
#include <CRC32.h>

#define MAX_ENTRIES 16

/**
 * @brief Batch structure to store multiple sensor entries with CRC.
 */
struct Batch
{
    uint16_t numEntries;
    SensorData entries[MAX_ENTRIES];
    uint32_t crc32; /**< CRC of timestamp + numEntries + entries */
};

struct SendStatusEntry {
    uint32_t timestamp;     // Unix time or millis()
    int32_t batchId;        // Batch ID
    bool success;           // true = OK, false = FAIL
    char message[40];      // Fixed-length message (null-terminated)
};

/**
 * @brief ESPLogger handles error logging and batch sensor logging to LittleFS.
 */
class ESPLogger
{
public:
    /** @brief Constructor */
    ESPLogger();

    /**
     * @brief Initializes LittleFS filesystem and prints status to Serial.  
     */
    void begin();

    // -------- Error logging --------
    /**
     * @brief Logs an error message to the error file with timestamp.
     * @param msg Error message string.
     */
    void logError(const String &msg);

    /**
     * @brief Prints all saved error messages to Serial.
     */
    void printErrors();

    /**
     * @brief Clears all stored error messages.
     */
    void clearErrors();

    // -------- Batch logging --------
    /**
     * @brief Logs a batch of sensor readings to LittleFS.
     * @param arr JSON array of sensor objects (must contain temperature, humidity, error, errorType)
     */
    void logBatch(std::vector<SensorData>& entries);

    /**
     * @brief Prints all valid batch files and their entries to Serial.
     */
    void printBatches();

    /**
     * @brief Retrieves the oldest valid batch from storage.
     * @param outEntries Vector to store sensor entries.
     * @param batchIndex Index of the batch file.
     * @return True if a valid batch was retrieved, false otherwise.
     */
    bool getOldestBatch(std::vector<SensorData> &outEntries, uint16_t &batchIndex);

    /**
     * @brief Retrieves the newest valid batch from storage.
     * @param outEntries Vector to store sensor entries.
     * @param batchIndex Index of the batch file.
     * @return True if a valid batch was retrieved, false otherwise.
     */
    bool getNewestBatch(std::vector<SensorData> &outEntries, uint16_t &batchIndex);

    /**
     * @brief Removes the oldest batch file from storage.
     */
    void removeOldestBatch();

    /**
     * @brief Deletes all batch files from storage.
     */
    void clearBatches();

    /**
     * @brief Log send status of a batch 
     */
    void logSendStatus(int batchId, bool success, const String &message);

    /**
     * @brief Print send status logs to Serial
     */
    void printSendStatusLogs();

    // -------- Batch file utilities --------
    /**
     * @brief Generates the filename for a batch index.
     * @param index Batch index number.
     * @return Full path string for the batch file.
     */
    String getBatchFilename(uint16_t index);

    /**
     * @brief Returns a sorted list of all batch indices currently stored.
     * @return Vector of batch indices.
     */
    std::vector<uint16_t> getBatchIndices();

    /**
     * @brief Reads a batch file, validates CRC and returns sensor entries.
     * @param fname Filename of the batch.
     * @param outEntries Vector to store sensor entries.
     * @return True if the batch is valid and read successfully.
     */
    bool readBatchFile(const String &fname, std::vector<SensorData> &outEntries);

    /**
     * @brief Prints a vector of sensor entries to Serial.
     * @param entries Vector of SensorEntry objects.
     */
    void printEntries(const std::vector<SensorData> &entries);

private:
    const char *ERROR_FILE = "/errors.txt";  /**< Path to error log file */
    const size_t MAX_BATCHES = 20;           /**< Maximum number of batch files stored */
};

#endif
