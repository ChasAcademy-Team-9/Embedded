#include <Arduino.h>
#include <DHT.h>
#include "MockSensor.h"
#include "log.h"
#include <WiFiS3.h>
//#include <WiFiClient.h> not needed?
#include <ArduinoJson.h>
#include "timeProvider.h"


#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "Chas Academy";
const char *password = "EverythingLouderThanEverythingElse";
const char *host = "ESP32_IP_ADDRESS";
const int port = 5000;

void setup()
{
  Serial.begin(115200);
  // randomSeed(analogRead(0));
  dht.begin();
  delay(4000);

  logStartup();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nArduino connected to WiFi");
}

void loop()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool error = false;
  String timestamp = getTimestamp();


  if (isnan(humidity) || isnan(temperature))
    error = true;
  // generateMockData(temperature, humidity, error);
  logSensorData(temperature, humidity, error);

  // Create JSON
  StaticJsonDocument<200> doc;
  doc["timestamp"] = timestamp;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["error"] = error;

  String jsonString;
  serializeJson(doc, jsonString);

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

  delay(2000);
}
