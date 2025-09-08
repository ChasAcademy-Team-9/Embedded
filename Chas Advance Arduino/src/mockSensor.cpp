#include "MockSensor.h"

void generateMockData(float &temp, float &hum, bool &error)
{
  // Slumpa mellan "fel" och "ok"
  int r = random(0, 10); // ca 20% chans på fel
  if (r < 2)
  {
    error = true;
    temp = -99; // orimligt värde
    hum = -1;
  }
  else
  {
    error = false;
    temp = random(220, 280) / 10.0; // 22.0–28.0 °C
    hum = random(400, 600) / 10.0;  // 40.0–60.0 %
  }
}
