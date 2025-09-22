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
Logger logger;

std::vector<SensorData> batchBuffer;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  delay(3000);
  Serial.println("Starting Arduino...");
  
 //Initialize logger
  logger.begin();
  logStartup();

  // Print all previous log entries
  logger.printAll();
  connectToESPAccessPoint();
}

void loop()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool error = false;

  if (isnan(humidity) || isnan(temperature))
    error = true;

  updateLogger();
  logSensorData(temperature, humidity, error);
  SensorData data = {temperature, humidity, error};
  batchSensorReadings(data);

  delay(2000);
}
