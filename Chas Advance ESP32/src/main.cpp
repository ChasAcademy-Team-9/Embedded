#include <Arduino.h>
#include "log.h"
#include "jsonParser.h"

String receiveMockupData()
{
  return R"({
    "timestamp": "2025-09-03 12:34:56",
    "temperature": 22.5,
    "humidity": 55.2,
    "error": false
  })";
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
  Serial.println("Starting up...");
  String json = receiveMockupData();
  parseJson(json);

  delay(2000);
}
