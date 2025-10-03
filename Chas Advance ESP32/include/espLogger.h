#ifndef ESPLOGGER_H
#define ESPLOGGER_H

#include "FS.h"
#include "LittleFS.h"
#include <ArduinoJson.h>
#include "sensorDataHandler.h"

class ESPLogger {
public:
    ESPLogger();
    void begin();

    // Error logging
    void logError(const String &msg);
    void printErrors();

    // Batch logging
    void logBatch(JsonArray arr);      
    void printBatches();               
    bool getOldestBatch(String &out);  
    void removeOldestBatch();          

    // Generic utils
    void clearErrors();
    void clearBatches();

private:
    const char *ERROR_FILE = "/errors.txt";
    const char *BATCH_FILE = "/batches.txt";
    const size_t MAX_BATCHES = 20;

    size_t countBatches();           
};

#endif