#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <Arduino.h>
#include "log.h"

void parseJson(String json);
void parseJsonArray(JsonArray arr, const String &timestamp);

#endif