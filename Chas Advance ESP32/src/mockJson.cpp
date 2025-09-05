#include "MockJson.h"
#include <Arduino.h>

String generateMockJson()
{
  char buf[32];
  float temp, hum;
  bool error;

  snprintf(buf, sizeof(buf), "2025-09-03 %02d:%02d:%02d", random(0, 24), random(0, 60), random(0, 60));

  int r = random(0, 10);

  if (r < 2)
  {
    error = true;
    temp = -99.0;
    hum = -1.0;
  }
  else
  {
    error = false;
    temp = random(220, 280) / 10.0;
    hum = random(400, 600) / 10.0;
  }

  String json = "{";
  json += "\"timestamp\": \"" + String(buf) + "\",";
  json += "\"temperature\": " + String(temp, 1) + ",";
  json += "\"humidity\": " + String(hum, 1) + ",";
  json += "\"error\": " + String(error ? "true" : "false");
  json += "}";

  return json;
}
