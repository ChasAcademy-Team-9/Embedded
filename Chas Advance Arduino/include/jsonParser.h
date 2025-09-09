#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <ArduinoJson.h>

String parseJSON(String timestamp, float temperature, float humidity, bool error);

#endif
