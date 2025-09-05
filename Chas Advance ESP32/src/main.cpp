#include <Arduino.h>
#include "log.h"
#include "jsonParser.h"
#include "mockJson.h"

void setup()
{
  Serial.begin(115200);
  logStartup();
  delay(1000);
}

void loop()
{
  String json = generateMockJson();
  parseJson(json);
  delay(2000);
}
