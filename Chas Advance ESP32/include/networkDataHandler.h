#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <WiFi.h>
#include <WiFiClient.h>
#include "sensorDataHandler.h"

/// Maximum allowed size (bytes) for incoming POST body to /data
#define maxRequestBodySize (10*1024) // 10 KB

/**
 * @brief Millis timestamp when the last data packet arrived from a sensor.
 */
extern unsigned long timeSinceDataReceived;

/**
 * @brief Non-blocking handler for incoming client connections.
 *
 * Reads request line and headers, validates POST /data, reads binary body,
 * and enqueues it for asynchronous processing. Sends immediate HTTP response.
 */
void handleClientAsync();

/**
 * @brief Validate the request line starts with "POST /data".
 * @param client Reference to WiFiClient to allow sending errors.
 * @param requestLine The first HTTP request line read from the socket.
 * @return true if valid POST /data request; false otherwise (and response sent).
 */
bool isValidPostRequest(WiFiClient &client, const String &requestLine);

/**
 * @brief Parse HTTP headers from the client and return Content-Length.
 * @param client Reference to WiFiClient to read headers from.
 * @return Content-Length value, or -1 if not found.
 */
int readContentLength(WiFiClient &client);

/**
 * @brief Read raw binary body into buffer.
 * @param client WiFiClient to read from.
 * @param buffer Vector to be resized and filled with data.
 * @param contentLength Expected number of bytes to read.
 * @return true if exactly contentLength bytes were read before timeout.
 */
bool readRequestBody(WiFiClient &client, std::vector<uint8_t> &buffer, int contentLength);

/**
 * @brief Send a simple HTTP response and close the client socket.
 * @param client Reference to the connected WiFiClient.
 * @param code HTTP status code to send (200 => OK, otherwise treated as Bad Request).
 */
void respond(WiFiClient &client, int code);

/**
 * @brief Initiate retries for saved batches persisted in LittleFS.
 *
 * Will attempt to post oldest saved batches and remove them on success.
 */
void trySendPendingBatches();

/**
 * @brief Low-level helper to POST JSON payload to backend server.
 * @param jsonString Body payload to send (JSON).
 * @param batchId Identifier used for logging send status; -1 if not from flash.
 * @return true on success (201 response), false otherwise.
 */
bool sendJsonToServer(const String &jsonString, int batchId);

/**
 * @brief High-level function to convert batch to JSON and send it.
 * @param batch Vector of SensorData to transmit.
 * @param batchID Batch identifier used for logging; -1 for ephemeral batches.
 * @return true if send succeeded; false otherwise.
 */
bool postBatchToServer(const std::vector<SensorData> &batch, int batchID);

/**
 * @brief Timestamp of the last received data (millis() value).
 * @note Defined in wifiHandler.cpp
 */

#endif