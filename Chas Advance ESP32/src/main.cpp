#include <Arduino.h>
#include "log.h"
#include "jsonParser.h"
#include "mockJson.h"
#include <WiFi.h>
#include "wifiHandler.h"

WifiHandler wifiHandler;
void setup()
{
  Serial.begin(115200);
  logStartup();
  delay(3000);

  Serial.println("Starting ESP32...");
  wifiHandler.init();
}

void loop()
{
  if((millis() - wifiHandler.timeSinceDataRecevied) > 5000){
    // If no data received for 5 seconds, generate warning

    logEvent("Insert TimeStamp", "ERROR", "No data recevied for 5 seconds", "FAIL");
    wifiHandler.timeSinceDataRecevied = millis(); // Reset timer
  }
  
  wifiHandler.server.handleClient();
  delay(1000);
}

