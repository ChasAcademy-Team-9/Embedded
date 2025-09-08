#include <Arduino.h>
#include <DHT.h>
#include "MockSensor.h"
#include "log.h"

#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  // randomSeed(analogRead(0));
  dht.begin();
  delay(1000);

  logStartup();
}

void loop()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool error = false;

  if (isnan(humidity) || isnan(temperature))
    error = true;
  // generateMockData(temperature, humidity, error);
  logSensorData(temperature, humidity, error);

  delay(2000);
}
