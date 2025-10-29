#include <WiFiS3.h>
#include "MockSensor.h"
#include "log.h"
#include "wifiHandler.h"
#include <vector>
#include "SensorData.h"
#include "batchHandler.h"
#include "arduinoLogger.h"

Logger logger;
TemperatureMode currentMode = ROOM_TEMP; // Default mode
uint8_t sensorId = 1;

// Track WiFi connection state for flash data transfer
bool wasWifiConnected = false;

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
  connectToESPAccessPointAsync();

  updateCurrentESPTime();

  if (isTimeInitialized)
  {
    SensorData data = measureSensorData(sensorId, currentESPTime, currentMode);
    if (batchSensorReadings(data) && attemptSendBatch())
    {
      sendDataToESP32(getBatchBuffer());
    }
    logSensorData(formatUnixTime(currentESPTime), data.temperature, data.humidity, static_cast<ErrorType>(data.errorType));
  }

  // Check for WiFi reconnection and send flash data only then
  bool isWifiConnected = (WiFi.status() == WL_CONNECTED);
  if (isWifiConnected && !wasWifiConnected)
  {
    Serial.println("WiFi reconnected! Checking for flash data to send...");
    logger.sendFlashDataIfAvailable(sensorId);
  }
  wasWifiConnected = isWifiConnected;

  updateLogger();

  delay(3000);
}
