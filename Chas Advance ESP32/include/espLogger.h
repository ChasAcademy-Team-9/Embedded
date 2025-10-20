#ifndef ESPLOGGER_H
#define ESPLOGGER_H

#include "FS.h"
#include "LittleFS.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "sensorData.h"
#include "log.h"
#include <CRC32.h>

#define MAX_ENTRIES 16

/**
 * @brief Persistent on-flash representation of a batch.
 *
 * Stored layout on disk:
 *  - uint16_t numEntries
 *  - SensorData[numEntries]
 *  - uint32_t crc32 (CRC of entries array)
 */
struct Batch
{
    uint16_t numEntries;
    SensorData entries[MAX_ENTRIES];
    uint32_t crc32; /**< CRC of timestamp + numEntries + entries */
};

/**
 * @brief Binary record used to log send attempts for batches.
 *
 * Stored compactly in /send_status.bin to save flash space.
 */
struct SendStatusEntry {
    uint32_t timestamp;     ///< Unix time or millis()
    int32_t batchId;        ///< Batch ID
    bool success;           ///< true = OK, false = FAIL
    char message[40];       ///< Fixed-length message (null-terminated)
};

// ----------------------------------------------------------------------------
/**
 * @class ESPLogger
 * @brief Helper for logging sensor batches and operational errors to LittleFS.
 *
 * Responsibilities:
 *  - Persist incoming sensor batches as binary files (/batch_<idx>.bin).
 *  - Maintain CRC for batch integrity and validate on read.
 *  - Record send-status events in a compact binary log.
 *  - Provide simple human-readable dumps to Serial for debugging.
 *  - Manage storage limits to MAX_BATCHES.
 */
class ESPLogger
{
public:
    /** @brief Constructor. */
    ESPLogger();

    /**
     * @brief Initialize LittleFS. Must be called once before other operations.
     *
     * Prints mount status to Serial.
     */
    void begin();

    // -------- Error logging --------
    /**
     * @brief Append an error message to the error log with a timestamp.
     * @param msg Human readable error message
     * @note This function is not used anywhere in the current codebase 
     *       - but kept for potential future use.
     */
    void logError(const String &msg);

    /** @brief Print all saved error messages to Serial (if any). */
    void printErrors();

    /** @brief Remove the error log file. */
    void clearErrors();

    // -------- Batch logging --------
    /**
     * @brief Store a vector of SensorData as a new batch file.
     *
     * The function writes: count (uint16_t) + raw entries + crc32(uint32_t).
     *
     * @param entries Vector of SensorData to persist. Must not be empty.
     */
    void logBatch(std::vector<SensorData> &entries);

    /** @brief Print all valid batches and their entries to Serial. */
    void printBatches();

    /**
     * @brief Retrieve the oldest valid batch from storage.
     * @param outEntries Output vector filled with the batch entries.
     * @param batchIndex Output index of the returned batch file.
     * @return true when a valid batch was found and populated.
     */
    bool getOldestBatch(std::vector<SensorData> &outEntries, uint16_t &batchIndex);

    /**
     * @brief Retrieve the newest valid batch from storage.
     * @param outEntries Output vector filled with the batch entries.
     * @param batchIndex Output index of the returned batch file.
     * @return true when a valid batch was found and populated.
     */
    bool getNewestBatch(std::vector<SensorData> &outEntries, uint16_t &batchIndex);

    /** @brief Delete the oldest batch file from storage. */
    void removeOldestBatch();

    /** @brief Remove all batch files. */
    void clearBatches();

    /** @brief Log the result of an attempt to send a batch to the backend. */
    void logSendStatus(int batchId, bool success, const String &message);

    /** @brief Print the send-status log to Serial (human readable). */
    void printSendStatusLogs();

    // -------- Batch file utilities --------
    /**
     * @brief Get batch filename for a given index.
     * @param index Batch index
     * @return Filename string (e.g. "/batch_3.bin")
     */
    String getBatchFilename(uint16_t index);

    /**
     * @brief Collect and sort stored batch indices.
     * @return Sorted vector of batch indices currently stored.
     */
    std::vector<uint16_t> getBatchIndices();

    /**
     * @brief Read a batch file and validate CRC before populating outEntries.
     * @param fname Filename to read
     * @param outEntries Output vector populated on success
     * @return true if the file was present, read and CRC matched
     */
    bool readBatchFile(const String &fname, std::vector<SensorData> &outEntries);

    /**
     * @brief Pretty-print an in-memory vector of SensorData to Serial.
     * @param entries Vector of SensorData
     */
    void printEntries(const std::vector<SensorData> &entries);

private:
    const char *ERROR_FILE = "/errors.txt";  /**< Path to error log file */
    const size_t MAX_BATCHES = 20;           /**< Max number of batch files kept */
};

#endif // ESPLOGGER_H
