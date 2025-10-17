#include "wifiHandler.h"
#include "arduinoLogger.h"
#include "ARDUINOSECRETS.h"

extern Logger logger;

bool wifiConnecting = false;
unsigned long wifiConnectStart = 0;
unsigned long lastRetrySendToESP32 = 0;
unsigned long maxSendRetryTime = 2000;
const uint8_t maxSendRetriesToESP32 = 3;
uint8_t attemptToESP32Count = 0;

#define MAX_FAILED_BATCHES 5
FailedBatch failedBatches[MAX_FAILED_BATCHES];
uint8_t failedBatchCount = 0;

void connectToESPAccessPointAsync()
{
    if (!wifiConnecting && WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, password);
        wifiConnecting = true;
        wifiConnectStart = millis();
        Serial.println("Starting WiFi connection...");
    }

    if (wifiConnecting)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            wifiConnecting = false;
            Serial.println("\nArduino connected to ESP32 Access Point");
        }
        else if (millis() - wifiConnectStart > 10000)
        { // timeout 10s
            wifiConnecting = false;
            Serial.println("\nWiFi connection timed out");
        }
    }
}

bool attemptSendBatch()
{
    return millis() - lastRetrySendToESP32 >= maxSendRetryTime;
}

void sendDataToESP32(std::vector<SensorData> &batch)
{
    bool success = false;

    if (attemptToESP32Count < maxSendRetriesToESP32)
    {
        lastRetrySendToESP32 = millis();
        attemptToESP32Count++;

        Serial.print("Sending batch attempt ");
        Serial.print(attemptToESP32Count);
        Serial.print("/");
        Serial.print(maxSendRetriesToESP32);
        Serial.println("...");

        if (postToESP32(batch))
        {
            batch.clear();
            resetBatchTimer();
            success = true;
            attemptToESP32Count = 0;
            return;
        }

        Serial.println("Send failed, retrying...");
    }

    if (!success && attemptToESP32Count >= maxSendRetriesToESP32)
    {
        Serial.print("ERROR: Batch send failed after ");
        Serial.print(maxSendRetriesToESP32);
        Serial.println(" attempts. Logging median");
        SensorData data = calculateMedian(batch);
        data.errorType = ErrorType::WiFi_FAIL;
        logger.log(String(data.temperature) + "," +
                   String(data.humidity) + "," +
                   String(static_cast<int>(data.errorType)));

        batch.clear();
        resetBatchTimer();
        attemptToESP32Count = 0; // Reset counter
    }
}

bool postToESP32(std::vector<SensorData> &batch)
{
    if (batch.empty())
        return false;
    // Simulate batch send failure
    // return false;
    WiFiClient client;

    if (!client.connect(host, port))
    {
        Serial.println("Connection to ESP32 failed");
        return false;
    }

    uint32_t sendMillis = millis();                                            // time of sending
    size_t totalSize = sizeof(sendMillis) + batch.size() * sizeof(SensorData); // Total size = 4 bytes sendMillis + n * sizeof(SensorData)

    // Send HTTP POST request to the esp
    client.print(String("POST /data HTTP/1.1\r\n") +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/octet-stream\r\n" +
                 "Content-Length: " + totalSize + "\r\n" +
                 "Connection: close\r\n\r\n");

    client.flush();                                                           // Ensure headers are sent before body
    client.write((uint8_t *)&sendMillis, sizeof(sendMillis));                 // Send sendMillis first
    client.write((uint8_t *)batch.data(), batch.size() * sizeof(SensorData)); // Send all SensorData
    client.flush();                                                           // Ensure body is sent

    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 2000)
    {
        if (client.available())
        {
            String line = client.readStringUntil('\n');
            if (line.startsWith("HTTP/1.1 200"))
            {
                Serial.println("POST successful!");
                client.stop();
                return true;
            }
        }
    }

    Serial.println("No valid response from ESP32");
    client.stop();
    return false;
}

void updateLogger()
{
    bool connected = (WiFi.status() == WL_CONNECTED);
    logger.update(connected);
}

// New functions for flash memory data transfer
void checkAndSendFlashData()
{
    Serial.println("Checking for flash data to send...");

    if (logger.size() > 0)
    {
        Serial.print("Found ");
        Serial.print(logger.size());
        Serial.println(" entries in flash memory. Attempting to send to ESP32...");

        // Create a vector of SensorData from flash memory
        std::vector<SensorData> flashBatch;
        for (size_t i = 0; i < logger.size(); i++)
        {
            String entryStr = logger.getEntry(i);
            if (entryStr.length() > 0)
            { // Only add non-empty entries
                // Parse the comma-separated string back to SensorData
                // Format: "temperature,humidity,errorType"
                int firstComma = entryStr.indexOf(',');
                int secondComma = entryStr.indexOf(',', firstComma + 1);

                if (firstComma != -1 && secondComma != -1)
                {
                    SensorData entry;
                    entry.SensorId = 1;         // Default sensor ID
                    entry.timestamp = millis(); // Use current time
                    entry.temperature = entryStr.substring(0, firstComma).toFloat();
                    entry.humidity = entryStr.substring(firstComma + 1, secondComma).toFloat();
                    entry.errorType = entryStr.substring(secondComma + 1).toInt();
                    entry.error = (entry.errorType != 0);

                    flashBatch.push_back(entry);
                }
            }
        }

        if (!flashBatch.empty())
        {
            Serial.print("Sending ");
            Serial.print(flashBatch.size());
            Serial.println(" flash entries using regular batch format");

            // Use the existing sendDataToESP32 function - same format as regular batches
            sendDataToESP32(flashBatch);

            // If send was successful (we assume it was since sendDataToESP32 doesn't return status)
            // Clear the flash memory after sending
            Serial.println("Flash data sent, clearing flash memory...");
            logger.clearAll();
        }
    }
    else
    {
        Serial.println("No data in flash memory to send");
    }
}