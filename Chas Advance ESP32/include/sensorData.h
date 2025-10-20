#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

/**
 * @brief Packed binary representation of a single sensor reading.
 *
 * The struct is packed to ensure stable on-wire / on-flash layout when
 * writing/reading binary batches.
 */
#pragma pack(push, 1)
struct SensorData
{
    uint8_t SensorId;    /**< ID of the sensor (0-255) */
    uint32_t timestamp;  /**< Unix timestamp (seconds) or sensor-local millis depending on context */
    float temperature;   /**< Temperature in degrees Celsius */
    float humidity;      /**< Relative humidity in percent */
    bool error;          /**< True if this reading indicates an error */
    uint8_t errorType;   /**< Error type code (see ErrorType enum in log.h) */
};
#pragma pack(pop)

#endif