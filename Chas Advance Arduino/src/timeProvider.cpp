#include "timeProvider.h"

String getTimestamp()
{
    static unsigned long counter = 0;
    counter += 1; // öka med 1 sekund per anrop
    int hours = (counter / 3600) % 24;
    int minutes = (counter / 60) % 60;
    int seconds = counter % 60;
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "2025-09-02 %02d:%02d:%02d", hours, minutes, seconds);
    return String(buffer);
}