#include "espLogger.h"

ESPLogger::ESPLogger() {}

void ESPLogger::begin()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed");
    }
    else
    {
        Serial.println("LittleFS mounted successfully");
    }
}

// -------- Error logging --------
void ESPLogger::logError(const String &msg)
{
    File f = LittleFS.open(ERROR_FILE, FILE_APPEND);
    if (!f)
    {
        Serial.println("Failed to open error log");
        return;
    }

    String timeStamp = getTimeStamp();
    f.printf("%s | Code:%d | %s\n", timeStamp.c_str(), msg.c_str());
    f.close();
}

// Batch logging
void ESPLogger::logBatch(JsonArray arr)
{
    // Check number of batches
    size_t n = countBatches();
    if (n >= MAX_BATCHES)
    {
        // Remove oldest - circular buffering
        removeOldestBatch();
    }

    File f = LittleFS.open(BATCH_FILE, FILE_APPEND);
    if (!f)
    {
        Serial.println("Failed to open batch log");
        return;
    }

    StaticJsonDocument<1024> doc;
    doc["timestamp"] = getTimeStamp();
    JsonArray data = doc.createNestedArray("data");

    for (JsonObject obj : arr)
    {
        JsonObject e = data.createNestedObject();
        e["t"] = obj["temperature"] | 0.0;
        e["h"] = obj["humidity"] | 0.0;
        e["err"] = obj["error"] | false;
        e["et"] = obj["errorType"] | 0;
    }

    String jsonStr;
    serializeJson(doc, jsonStr);

    f.println(jsonStr);
    f.close();
}

void ESPLogger::printBatches()
{
    File f = LittleFS.open(BATCH_FILE, FILE_READ);
    if (!f)
    {
        Serial.println("No batch log found");
        return;
    }
    Serial.println("---- Batch log start ----");
    while (f.available())
    {
        Serial.println(f.readStringUntil('\n'));
    }
    Serial.println("---- Batch log end ----");
    f.close();
}

void ESPLogger::printErrors()
{
    File f = LittleFS.open(ERROR_FILE, FILE_READ);
    if (!f)
    {
        Serial.println("No Error log found");
        return;
    }
    Serial.println("---- Error log start ----");
    while (f.available())
    {
        Serial.println(f.readStringUntil('\n'));
    }
    Serial.println("---- Error log end ----");
    f.close();
}

bool ESPLogger::getOldestBatch(String &out)
{
    File f = LittleFS.open(BATCH_FILE, FILE_READ);
    if (!f)
        return false;

    out = f.readStringUntil('\n');
    f.close();
    return out.length() > 0;
}

void ESPLogger::removeOldestBatch()
{
    File f = LittleFS.open(BATCH_FILE, FILE_READ);
    if (!f)
        return;

    String rest = "";
    f.readStringUntil('\n'); // Skip first line
    while (f.available())
    {
        rest += f.readStringUntil('\n') + "\n"; // Save every line
    }
    f.close();

    File fw = LittleFS.open(BATCH_FILE, FILE_WRITE);
    fw.print(rest); // Write all lines except first(oldest)
    fw.close();
}

// Utils
void ESPLogger::clearErrors()
{
    LittleFS.remove(ERROR_FILE);
}

void ESPLogger::clearBatches()
{
    LittleFS.remove(BATCH_FILE);
}

size_t ESPLogger::countBatches()
{
    File f = LittleFS.open(BATCH_FILE, FILE_READ);
    if (!f)
        return 0;
    size_t n = 0;
    while (f.available())
    {
        f.readStringUntil('\n');
        n++;
    }
    f.close();
    return n;
}
