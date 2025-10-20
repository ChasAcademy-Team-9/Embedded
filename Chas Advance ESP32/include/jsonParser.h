#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <Arduino.h>
#include "sensorData.h"
#include "log.h"

/**
 * @brief Convert a vector of SensorData entries into a JSON array string.
 *
 * Produces a JSON document containing an array of objects with keys:
 *  - "ArduinoID"
 *  - "SensorTimeStamp" (formatted via formatUnixTime)
 *  - "Temperature"
 *  - "Humidity"
 *
 * @param batch Vector of SensorData entries to serialize
 * @return String containing serialized JSON document (array)
 */
String serializeBatchToJson(const std::vector<SensorData> &batch);

#endif // JSONPARSER_H