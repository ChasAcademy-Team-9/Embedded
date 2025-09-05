#include <Arduino.h>
#include "MockSensor.h"
#include "log.h"

void setup()
{
  Serial.begin(115200);
  randomSeed(analogRead(0));
  delay(1000);

  logStartup();
}

void loop()
{
  float temperature, humidity;
  bool error;

  generateMockData(temperature, humidity, error);
  logSensorData(temperature, humidity, error);

  delay(2000);
}
