#include "wifiHandler.h"
#include "arduinoLogger.h"
#include "ARDUINOSECRETS.h"

extern Logger logger;

// Wifi
bool isWifiConnecting = false;
unsigned long wifiConnectStartMillis = 0;

// Data (network)
unsigned long lastSendRetryMillis = 0;
unsigned long sendRetryIntervalMs = 2000;
const uint8_t maxSendRetryCount = 3;
uint8_t currentSendAttemptCount = 0;

// Time
uint32_t currentESPTime = 0;
bool shouldGetEspTime = true;
bool isTimeInitialized = false;
unsigned long lastUpdateMillis = 0;
unsigned long lastTimeSyncMillis = 0;
unsigned long timeSyncIntervalMs = 10 * 60 * 1000; //10 minutes

void connectToESPAccessPointAsync()
{
    if (!isWifiConnecting && WiFi.status() != WL_CONNECTED)
    {
        shouldGetEspTime = true;
        WiFi.begin(ssid, password);
        isWifiConnecting = true;
        wifiConnectStartMillis = millis();
        Serial.println("\033[33mStarting WiFi connection...\033[0m");
    }

    if (isWifiConnecting)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            isWifiConnecting = false;
            Serial.println("\n\033[32mArduino connected to ESP32 Access Point\033[0m");
        }
        else if (millis() - wifiConnectStartMillis > 10000)
        { // timeout 10s
            isWifiConnecting = false;
            Serial.println("\n\033[33mWiFi connection timed out");
        }
    }
    if (WiFi.status() == WL_CONNECTED && shouldGetEspTime)
    {
        if (!getTimeFromESP32())
        {
            return;
        }
        Serial.print("Updated time from ESP32: ");
        Serial.println(formatUnixTime(currentESPTime));
    }
}

bool attemptSendBatch()
{
    return millis() - lastSendRetryMillis >= sendRetryIntervalMs;
}

bool sendDataToESP32(std::vector<SensorData> &batch)
{
    bool success = false;

    if (currentSendAttemptCount < maxSendRetryCount)
    {
        lastSendRetryMillis = millis();
        currentSendAttemptCount++;

        Serial.print("Sending batch. Attempt: ");
        Serial.print(currentSendAttemptCount);
        Serial.print("/");
        Serial.print(maxSendRetryCount);
        Serial.println("...");

        if (postToESP32(batch))
        {
            batch.clear();
            resetBatchTimer();
            success = true;
            currentSendAttemptCount = 0;
            return true;
        }

        Serial.println("Send failed, retrying...");
    }

    if (!success && currentSendAttemptCount >= maxSendRetryCount)
    {
        Serial.print("\033[31mERROR: Batch send failed after ");
        Serial.print(maxSendRetryCount);
        Serial.println(" \033[31mattempts. Logging median\033[0m");

        SensorData data = calculateMedian(batch);
        data.errorType = ErrorType::WiFi_FAIL;
        logger.logDataEntry(data);

        batch.clear();
        resetBatchTimer();
        currentSendAttemptCount = 0;
        return false;
    }

    return false;
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
        Serial.println("\033[31mConnection to ESP32 failed when posting to ESP\033[0m");
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
                Serial.println("\033[32mPOST successful!\033[0m");
                client.stop();
                return true;
            }
        }
    }

    Serial.println("\033[31mNo valid response from ESP32\033[0m");
    client.stop();
    return false;
}

void updateLogger()
{
    bool connected = (WiFi.status() == WL_CONNECTED);
    logger.update(connected);
}

bool getTimeFromESP32()
{
    shouldGetEspTime = true;

    WiFiClient client;
    if (!client.connect(host, port))
    {
        Serial.println("\033[31mConneciton to ESP failed when attempting to get time\033[0m");
        return false;
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
            // Protocol: server must send timestamp as 4 bytes (uint32_t, little-endian)
            client.read((uint8_t *)&epochTime, sizeof(uint32_t));            
            client.stop();
            // Optional sanity check
            if (epochTime == 0 || epochTime > 4102444800UL) // ~year 2100
            {
                Serial.print("\033[31mInvalid time received from ESP32: \033[0m");
                Serial.println(epochTime);
                return false;
            }
            Serial.print("\033[32mGot valid time from ESP: \033[0m");
            Serial.println(epochTime);

            shouldGetEspTime = false;
            isTimeInitialized = true;
            lastTimeSyncMillis = millis();
            currentESPTime = epochTime;
            return true;
        }
        delay(10); // Give time for more data to arrive
    }

    client.stop();
    Serial.println("\033[31mNo valid response from ESP32 when getting time\033[0m");
    return false;
}

// Called in loop
void updateCurrentESPTime()
{
    unsigned long now = millis();
    unsigned long deltaMs = now - lastUpdateMillis; // elapsed since last call
    lastUpdateMillis = now;

    currentESPTime += (deltaMs / 1000); // increment seconds

    if (now - lastTimeSyncMillis > timeSyncIntervalMs)
    {
        getTimeFromESP32();
    }
}
