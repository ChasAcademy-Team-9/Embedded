#include <Arduino.h>
#include <DHT.h>
#include "MockSensor.h"
#include "log.h"
#include "wifiHandler.h"
#include "jsonParser.h"

#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  // randomSeed(analogRead(0));
  dht.begin();
  delay(4000);

  logStartup();

  connectToWiFi();
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
  logSensorData(timestamp, temperature, humidity, error);
  connectToESP(parseJSON(timestamp, temperature, humidity, error));

  delay(2000);
}
