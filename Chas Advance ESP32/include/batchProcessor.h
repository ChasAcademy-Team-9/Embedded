#ifndef BATCHPROCESSOR_H
#define BATCHPROCESSOR_H

#include <Arduino.h>
#include "sensorDataHandler.h"
#include "log.h"

/**
 * @brief Internal structure representing an incoming batch from a client.
 *
 * data: raw bytes received (sendMillis + SensorData[])
 * clientIP: IPv4 address in network order returned by WiFiClient::remoteIP()
 */
struct IncomingBatch
{
  std::vector<uint8_t> data;
  uint32_t clientIP;
};

/**
 * @brief Handle a parsed sensor batch: assign timestamps, log entries and try send.
 */
static void handleParsedBatch(uint32_t sendMillis, std::vector<SensorData> &sensorBatch);

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
static void processSingleBatch(const IncomingBatch &incoming);

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