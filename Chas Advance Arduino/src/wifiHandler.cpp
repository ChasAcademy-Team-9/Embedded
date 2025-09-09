#include "wifiHandler.h"
#include "ARDUINOSECRETS.h"

void connectToWiFi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nArduino connected to WiFi");
}

void connectToESP(String jsonString)
{
    WiFiClient client;

    if (!client.connect(host, port))
    {
        Serial.println("Connection failed");
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
