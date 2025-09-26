#include "wifiHandler.h"
#include "arduinoLogger.h"
#include "ARDUINOSECRETS.h"
#include "batchHandler.h"
#include "jsonParser.h"

extern Logger logger;

bool wifiConnecting = false;
unsigned long wifiConnectStart = 0;

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

void sendDataToESP32(String jsonString)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected, reconnecting...");
        connectToESPAccessPointAsync();
    }

    const int maxRetries = 3;
    bool success = false;

    for (int attempt = 1; attempt <= maxRetries; attempt++)
    {
        Serial.print("Sending batch attempt ");
        Serial.print(attempt);
        Serial.print("/");
        Serial.print(maxRetries);
        Serial.println("...");

        if (postToESP32(jsonString))
        {
            success = true;
            break;
        }

        Serial.println("Send failed, retrying...");
        delay(2000); // Wait before retrying
    }

    if (!success)
    {
        Serial.println("ERROR: Batch send failed after 3 attempts.");
        // Log error in EEPROM, optional
        // logger.log("Batch send failed after 3 attempts");
        if (failedBatchCount < MAX_FAILED_BATCHES)
        {
            failedBatches[failedBatchCount++] = {jsonString, 1};
            Serial.println("Batch queued for later retry");
        }
        else
        {
            Serial.println("Queue full, batch discarded");
        }
    }
}

bool postToESP32(const String &jsonString)
{
    // Simulate batch send failure
    // return false;
    WiFiClient client;

    if (!client.connect(host, port))
    {
        Serial.println("Connection to ESP32 failed");
        return false;
    }

    // Send HTTP POST request to the esp
    client.print(String("POST /data HTTP/1.1\r\n") +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + jsonString.length() + "\r\n" +
                 "Connection: close\r\n\r\n" +
                 jsonString);

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

void retryFailedBatches()
{
    for (int i = 0; i < failedBatchCount; i++)
    {
        FailedBatch &batch = failedBatches[i];
        if (postToESP32(batch.json))
        {
            Serial.println("Retry succeeded for batch");
            // Remove batch from queue
            for (int j = i; j < failedBatchCount - 1; j++)
                failedBatches[j] = failedBatches[j + 1];
            failedBatchCount--; // Reduce batches by one
            i--;                // adjust index
        }
        else
        {
            batch.retries++;
            if (batch.retries >= 3)
            {
                Serial.println("Retry limit reached, discarding batch");
                SensorData data = getDataFromJson(batch.json);
                data.errorType = ErrorType::WiFi_FAIL;
                logger.log(String(data.temperature) + "," +
                           String(data.humidity) + "0" + 
                           String(static_cast<int>(data.errorType)));
                for (int j = i; j < failedBatchCount - 1; j++)
                    failedBatches[j] = failedBatches[j + 1];
                failedBatchCount--;
                i--;
            }
        }
    }
}

void updateLogger()
{
    bool connected = (WiFi.status() == WL_CONNECTED);
    logger.update(connected);
}