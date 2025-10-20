#include "batchProcessor.h"
#include "log.h"
#include <queue>
#include <mutex>
#include "networkDataHandler.h"
#include "espLogger.h"

/* Global queue and mutex used to transfer work from the network handler
 * (handleClientAsync) to the background processing task (processBatches).
 * Accesses must hold queueMutex. */
std::queue<IncomingBatch> batchQueue;
std::mutex queueMutex;

/* External logger instance for logging batches to flash */
extern ESPLogger logger;

/**
 * @brief Handle a parsed sensor batch: assign timestamps, log entries and try send.
 */
static void handleParsedBatch(uint32_t sendMillis, std::vector<SensorData> &sensorBatch)
{
  assignAbsoluteTimestamps(sendMillis, sensorBatch);
  Serial.printf("Received batch with %u entries\n", (unsigned) sensorBatch.size());

  for (const auto &entry : sensorBatch)
  {
    logSensorData(formatUnixTime(entry.timestamp),
                  entry.temperature,
                  entry.humidity,
                  static_cast<ErrorType>(entry.errorType));
  }

  if (!postBatchToServer(sensorBatch, -1))
  {
    Serial.println("Failed to send batch to backend server - saving in flash");
    logger.logBatch(sensorBatch);

    uint16_t batchIndex = 0;
    if (logger.getNewestBatch(sensorBatch, batchIndex))
    {
      logger.logSendStatus(batchIndex, false, "Failed send");
    }
  }
  else
  {
    Serial.println("Batch received from sensor was sent successfully to backend server");
  }
}

/**
 * @brief Parse a binary batch payload into sendMillis and SensorData array.
 *
 * The function validates size and layout before copying the entries.
 *
 * @param buffer Input bytes.
 * @param sendMillis (out) extracted uint32_t sendMillis value.
 * @param batch (out) parsed SensorData entries.
 * @return true on success, false if buffer too small or misaligned.
 */
bool parseBatch(const std::vector<uint8_t> &buffer, uint32_t &sendMillis, std::vector<SensorData> &batch)
{
  if (buffer.size() < sizeof(sendMillis))
    return false;

  // Extract sendMillis (first 4 bytes)
  memcpy(&sendMillis, buffer.data(), sizeof(sendMillis));

  // Remaining data should be a multiple of SensorData struct size
  size_t dataSize = buffer.size() - sizeof(sendMillis);
  if (dataSize % sizeof(SensorData) != 0)
    return false;

  // Copy SensorData entries into batch vector
  batch.resize(dataSize / sizeof(SensorData));
  memcpy(batch.data(), buffer.data() + sizeof(sendMillis), dataSize);
  return true;
}

/**
 * @brief Process a single queued IncomingBatch.
 *
 * Responsible for parsing the binary payload and delegating to the parsed
 * batch handler or logging an error when parsing fails.
 */
static void processSingleBatch(const IncomingBatch &incoming)
{
  uint32_t sendMillis;
  std::vector<SensorData> sensorBatch;
  if (parseBatch(incoming.data, sendMillis, sensorBatch))
  {
    handleParsedBatch(sendMillis, sensorBatch);
  }
  else
  {
    Serial.println("Invalid batch received - batch discarded");
  }
}

/**
 * @brief Background loop that consumes queued IncomingBatch items.
 */
void processBatches(void *parameter)
{
  while (true)
  {
    IncomingBatch batch;
    bool hasBatch = false;

    {
      std::lock_guard<std::mutex> lock(queueMutex);
      if (!batchQueue.empty())
      {
        batch = batchQueue.front();
        batchQueue.pop();
        hasBatch = true;
      }
    }

    if (hasBatch)
    {
      processSingleBatch(batch);
    }
    else
    {
      vTaskDelay(100 / portTICK_PERIOD_MS); // small delay if queue empty
    }
  }
}