#include <Arduino.h>
#include <DHT.h>
#include "MockSensor.h"
#include "log.h"
#include "wifiHandler.h"
#include "jsonParser.h"
#include <vector>
#include "SensorData.h"
#include "batchHandler.h"

#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

std::vector<SensorData> batchBuffer;
unsigned long batchStartTime = 0;

void setup()
{
  Serial.begin(115200);
  // randomSeed(analogRead(0));
  dht.begin();
  delay(4000);

  logStartup();

  connectToESPAccessPoint();
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
  SensorData data = {temperature, humidity, error};
  batchSensorReadings(data);

  delay(2000);
}
