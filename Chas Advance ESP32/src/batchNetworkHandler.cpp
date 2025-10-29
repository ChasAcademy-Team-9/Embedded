#include "batchNetworkHandler.h"
#include "ESPSECRETS.h"
#include "espLogger.h"
#include <queue>
#include <mutex>
#include <HTTPClient.h>
#include "jsonParser.h"
#include "batchProcessor.h"

extern std::queue<IncomingBatch> batchQueue;
extern std::mutex queueMutex;

/* API batch send variables */
unsigned long lastRetryTime = 0;
const unsigned long retryInterval = 10000; ///< ms between retry attempts
bool retryInProgress = false;
/**
 * @brief Millis timestamp when the last data packet arrived.
 */
unsigned long timeSinceDataReceived = 0;

/**
 * @brief External logger instance used for persistence and send-status logging.
 * Declared elsewhere (typically in main or a dedicated logger module).
 */
extern ESPLogger logger;
extern WiFiServer server;

/**
 * @brief Poll server.available() and handle a single connection if present.
 *
 * Called frequently from the main loop. Forwards successful POST/GET payloads to
 * handler-functions.
 * 
 */
void handleClientAsync()
{
  WiFiClient client = server.available();
  if (!client)
    return;

  String requestLine = client.readStringUntil('\n');
  requestLine.trim();
  if (requestLine.startsWith("POST /data"))
  {
    handlePostRequest(client);
  }
  else if (requestLine.startsWith("GET /time"))
  {
    handleGetTimeRequest(client);
  }
  else
  {
    client.println("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
    client.stop();
    return ;
  }
}

/**
 * @brief Handle a valid POST /data request from client.
 *
 * Reads headers, validates Content-Length, reads binary body, enqueues for
 * processing, and sends immediate HTTP response.
 * @param client Reference to connected WiFiClient.
 */

void handlePostRequest(WiFiClient &client)
{
  // Step 1: Read headers and Content-Length
  int contentLength = readContentLength(client);
  if (contentLength <= 0 || contentLength > maxRequestBodySize)
  {
    respond(client, 400);
    return;
  }

  // Step 2: Read binary body
  std::vector<uint8_t> buffer;
  if (!readRequestBody(client, buffer, contentLength))
  {
    respond(client, 400);
    return;
  }

  // Step 3: Push to processing queue
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (batchQueue.size() >= MaxBatchQueueSize)
    {
      Serial.println("Batch queue full, discarding incoming batch");
      respond(client, 503); // Service unavailable
      return;
    }
    batchQueue.push({buffer});
  }

  // Step 4: Immediate response
  respond(client, 200);
  timeSinceDataReceived = millis(); // Update last-received timestamp
}

/**
 * @brief Handle a GET /time request from client.
 * 
 * Responds with current time as plain text.
 * @param client Reference to connected WiFiClient.
 */
void handleGetTimeRequest(WiFiClient &client)
{
    time_t now;
    time(&now); // current UNIX time (UTC)
    uint32_t epoch = (uint32_t)now; // truncate to 32 bits

    if (now == 0)
    {
      Serial.println("Failed to get current time");
        client.println("HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n");
        client.stop();
        return;
    }

    // Send HTTP headers
    Serial.print("Sending current time to client: ");
    Serial.println(formatUnixTime(epoch));
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/octet-stream"); // raw bytes
    client.println("Content-Length: " + String(sizeof(epoch)));
    client.println("Connection: close");
    client.println();

    // Send time as binary
    client.write((uint8_t*)&epoch, sizeof(epoch));
    client.flush();
    client.stop();
}

/**
 * @brief Try to send saved batches from storage with retry/backoff logic.
 *
 * Locally keeps state (static variables) to track current attempt and index.
 * Removes batch file on success and logs final failure after 3 attempts.
 */
void trySendPendingBatches()
{
  static std::vector<SensorData> batchEntries;
  static uint16_t batchIndex = 0;
  static int attempt = 0;

  if (!retryInProgress)
  {
    if (!logger.getOldestBatch(batchEntries, batchIndex))
      return;
    attempt = 0;
    retryInProgress = true;
  }

  if (millis() - lastRetryTime < retryInterval)
    return;
  lastRetryTime = millis();
  attempt++;

  Serial.printf("Sending saved batch %d attempt %d/%d...\n", batchIndex, attempt, 3);

  if (postBatchToServer(batchEntries, batchIndex))
  {
    Serial.println("Saved batch sent successfully, removing file");
    String fname = logger.getBatchFilename(batchIndex);
    LittleFS.remove(fname);
    retryInProgress = false;
    batchEntries.clear();
    return;
  }

  if (attempt >= 3)
  {
    Serial.println("Sending saved batch failed after 3 retries, will retry later");
    logger.logSendStatus(batchIndex, false, "3 retries failed");
    retryInProgress = false;
  }
}

/**
 * @brief Convert a SensorData batch to JSON and send to server.
 *
 * @param batch Vector of SensorData entries to transmit.
 * @param batchId Identifier used in send-status logging.
 * @return true if sendJsonToServer reports success, false otherwise.
 */
bool postBatchToServer(const std::vector<SensorData> &batch, int batchId)
{
  if (batch.empty())
  {
    Serial.println("Batch is empty, not sending.");
    return false;
  }

  String jsonString = serializeBatchToJson(batch);
  return sendJsonToServer(jsonString, batchId);
}

/**
 * @brief Send JSON string to backend using HTTPClient and log response.
 *
 * The current check expects HTTP 201 to treat the send as successful.
 *
 * @param jsonString Body to POST.
 * @param batchId Identifier used for logging (can be -1 for transient).
 * @return true on HTTP 201; false otherwise.
 */
bool sendJsonToServer(const String &jsonString, int batchId)
{
  // return false; // For testing without server
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected - can't send batch to database!");
    logger.logSendStatus(batchId, false, "WiFi not connected");
    return false;
  }

  HTTPClient http;
  http.begin(host); // HTTPS URL
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0)
  {
    String payload = http.getString(); // response from server
    if (httpResponseCode == 201)
    {
      logger.logSendStatus(batchId, true, "OK");
      Serial.println("Batch sent successfully!");
      http.end();
      return true;
    }
    else
    {
      logger.logSendStatus(batchId, false, payload);
      Serial.print("Server responded with code: ");
      Serial.println(httpResponseCode);
      http.end();
      return false;
    }
  }
  else
  {
    Serial.print("Error sending POST: ");
    Serial.println(httpResponseCode);
    logger.logSendStatus(batchId, false, "Connection failed");
    http.end();
    return false;
  }
}

//---------Helper functions for handleClientAsync() ---------
/**
 * @brief Read HTTP headers until the blank line and extract Content-Length.
 *
 * @param client Client to read the headers from.
 * @return Parsed Content-Length value or -1 if header missing.
 */
int readContentLength(WiFiClient &client)
{
  int contentLength = -1;
  while (client.connected())
  {
    String line = client.readStringUntil('\n');
    line.trim();
    if (line.length() == 0)
      break; // empty line means end of headers
    if (line.startsWith("Content-Length:"))
    {
      contentLength = line.substring(15).toInt();
    }
  }
  return contentLength;
}

/**
 * @brief Read contentLength bytes from the client into buffer (binary-safe).
 *
 * Uses a 3s overall timeout to prevent blocking forever on partial uploads.
 *
 * @param client WiFiClient to read from.
 * @param buffer Output buffer resized to contentLength and filled.
 * @param contentLength Expected number of bytes.
 * @return true if exactly contentLength bytes were read; false on timeout/error.
 */
bool readRequestBody(WiFiClient &client, std::vector<uint8_t> &buffer, int contentLength)
{
  buffer.resize(contentLength);
  int bytesRead = 0;
  unsigned long start = millis();

  // Keep reading until all bytes received or 3s timeout
  while (bytesRead < contentLength && millis() - start < 3000)
  {
    if (client.available())
    {
      bytesRead += client.read(buffer.data() + bytesRead, contentLength - bytesRead);
    }
  }
  return bytesRead == contentLength;
}

/**
 * @brief Send a minimal HTTP response and close the connection.
 *
 * 200 -> "200 OK", any other code is reported as "Bad Request" with the code.
 *
 * @param client Client to reply to.
 * @param code HTTP response code (200 for OK).
 */
void respond(WiFiClient &client, int code)
{
  if (code == 200)
  {
    client.println("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
  }
  else
  {
    client.printf("HTTP/1.1 %d Bad Request\r\nConnection: close\r\n\r\n", code);
  }
  client.stop();
}