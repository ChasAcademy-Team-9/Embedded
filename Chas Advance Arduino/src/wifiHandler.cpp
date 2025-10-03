#include "wifiHandler.h"
#include "arduinoLogger.h"
#include "ARDUINOSECRETS.h"

extern Logger logger;

bool wifiConnecting = false;
unsigned long wifiConnectStart = 0;

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

    WiFiClient client;

    if (!client.connect(host, port))
    {
        Serial.println("Connection to ESP32 failed");
        delay(2000);
        return;
    }

    // Send HTTP POST request to the esp
    client.print(String("POST /data HTTP/1.1\r\n") +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + jsonString.length() + "\r\n" +
                 "Connection: close\r\n\r\n" +
                 jsonString);

    client.stop();
}

void updateLogger()
{
    bool connected = (WiFi.status() == WL_CONNECTED);
    logger.update(connected);
}