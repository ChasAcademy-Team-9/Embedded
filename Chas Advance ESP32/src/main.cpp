#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
  Serial.println("Hello, World! I'm ESP32");

  delay(2000);
}
