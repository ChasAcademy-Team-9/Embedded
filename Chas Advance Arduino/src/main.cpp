#include <DHT.h>
#include "MockSensor.h"
#include "log.h"
#include "wifiHandler.h"
#include "jsonParser.h"

#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Logger logger;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  delay(3000);
  
  Serial.println("Starting Arduino...");
  #ifdef ARDUINO_UNOR4_WIFI
  Serial.println("UNO R4 WiFi detected");
  #endif
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

  logSensorData(temperature, humidity, error);
  sendDataToESP32(parseJSON(temperature, humidity, error));

  delay(2000);
}
