#ifndef BATCHPROCESSOR_H
#define BATCHPROCESSOR_H

#include <Arduino.h>
#include "sensorData.h"
#include "log.h"

/**
 * @brief Internal structure representing an incoming batch from a client.
 *
 * data: raw bytes received (sendMillis + SensorData[])
 */
struct IncomingBatch
{
  std::vector<uint8_t> data;
};
#define MaxBatchQueueSize 15

/**
 * @brief Handle a parsed sensor batch: assign timestamps, log entries and try send.
 */
void handleParsedBatch(uint32_t sendMillis, std::vector<SensorData> &sensorBatch);

/**
 * @brief Parse binary payload into sendMillis and SensorData entries.
 *
 * Expected layout:
 *  - 4 bytes: sendMillis (uint32_t)
 *  - remaining bytes: array of SensorData (packed as sizeof(SensorData))
 *
 * @param buffer Incoming binary buffer.
 * @param sendMillis (out) extracted sendMillis.
 * @param batch (out) vector filled with SensorData entries.
 * @return true if parsing succeeded; false on invalid payload.
 */
bool parseBatch(const std::vector<uint8_t> &buffer, uint32_t &sendMillis, std::vector<SensorData> &batch);

/**
 * @brief Process a single queued IncomingBatch.
 *
 * Responsible for parsing the binary payload and delegating to the parsed
 * batch handler or logging an error when parsing fails.
 */
void processSingleBatch(const IncomingBatch &incoming);

/**
 * @brief Background task that processes queued incoming batches.
 *
 * The task:
 *  - pops batches from the queue,
 *  - processes them via processSingleBatch(),
 *
 * @param parameter Not used (task parameter).
 */
void processBatches(void *parameter);

#endif