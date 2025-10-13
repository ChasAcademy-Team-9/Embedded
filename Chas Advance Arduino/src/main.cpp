#include <DHT.h>
#include "MockSensor.h"
#include "log.h"
#include "wifiHandler.h"
#include <vector>
#include "SensorData.h"
#include "batchHandler.h"
#include "arduinoLogger.h"

#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Logger logger;
TemperatureMode currentMode = ROOM_TEMP; // Default mode
uint8_t sensorId = 1;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  delay(3000);
  Serial.println("Starting Arduino...");

  // Initialize logger
  logger.begin();
  logStartup();

  // Print all previous log entries
  logger.printAll();
}

void loop()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool error = false;

  SensorData data = {sensorId, millis(), temperature, humidity, error, NONE};
  if (isnan(humidity) || isnan(temperature))
  {
    data.error = true;
    data.errorType = SENSOR_FAIL;
  }
  if (!data.error)
  {
    checkThresholds(data, getThresholdsForMode(currentMode));
  }
  connectToESPAccessPointAsync();
  updateLogger();

  if (batchSensorReadings(data) && attemptSendBatch())
  {
    sendDataToESP32(getBatchBuffer());
  }

  logSensorData(data.temperature, data.humidity, static_cast<ErrorType>(data.errorType));


  delay(3000);
}
