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
uint32_t currentESPTime = 0;
bool hasESPTime = false;
unsigned long lastUpdateMillis = 0;

#define MAX_FAILED_BATCHES 5
FailedBatch failedBatches[MAX_FAILED_BATCHES];
uint8_t failedBatchCount = 0;

void connectToESPAccessPointAsync()
{
    if (!wifiConnecting && WiFi.status() != WL_CONNECTED)
    {
        hasESPTime = false;
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
    if(WiFi.status() == WL_CONNECTED && !hasESPTime) {
        currentESPTime = getTimeFromESP32();
        Serial.print("Updated time from ESP32: ");
        Serial.println(formatUnixTime(currentESPTime));
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
        logger.logDataEntry(data);

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

uint32_t getTimeFromESP32()
{
    hasESPTime = false;

    WiFiClient client;
    if (!client.connect(host, port))
    {
        Serial.println("Connection failed");
        return 0;
    }

    // Send GET request
    client.print(String("GET /time HTTP/1.1\r\n") +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long start = millis();
    bool headersEnded = false;

    while (millis() - start < 3000)
    {
        // Skip headers line by line
        while (client.available() && !headersEnded)
        {
            String line = client.readStringUntil('\n');
            line.trim();
            if (line.length() == 0)
            {
                headersEnded = true; // empty line -> end of headers
            }
        }

        // After headers, read 4 bytes
        if (headersEnded && client.available() >= 4)
        {
            uint32_t epochTime = 0;
            client.read((uint8_t*)&epochTime, sizeof(epochTime));
            client.stop();
            hasESPTime = true;
            return epochTime;
        }
    }

    client.stop();
    Serial.println("No valid response");
    return 0;
}


void updateCurrentESPTime()
{
    unsigned long now = millis();
    unsigned long deltaMs = now - lastUpdateMillis;  // elapsed since last call
    lastUpdateMillis = now;

    currentESPTime += (deltaMs / 1000);  // increment seconds
}
