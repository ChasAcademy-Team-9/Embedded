/**
 * @file wifiHandler.h
 * @brief HTTP ingestion, WiFi and batch-handling public API for the ESP32 gateway.
 *
 * This header exposes functions used to initialize and run the network-facing
 * parts of the firmware: STA/AP setup, a simple HTTP server that accepts POST
 * /data binary payloads and queue-based batch processing + retry helpers.
 *
 * The implementation is designed to:
 *  - accept binary batches from sensors (sendMillis + SensorData[]),
 *  - enqueue them for processing by a background task,
 *  - persist failed batches and retry sending to backend.
 *
 * Note: many helper functions operate on Arduino/ESP32-specific types
 * (WiFiClient, WiFiServer). This header is intended for firmware builds.
 */

#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "jsonParser.h"
#include "log.h"
#include <time.h>

/**
 * @brief Initialize WiFi, access point and HTTP server, and configure NTP.
 *
 * Internally calls connectToWiFi(),
 * setupAccessPoint(), setupHttpServer(), and configTime().
 */
void initWifi();

/**
 * @brief Connect the ESP32 to configured STA WiFi network.
 *
 * Blocks until WiFi connected in current implementation.
 */
void connectToWiFi();

/**
 * @brief Configure the device as an Access Point for local device pairing.
 *
 * Starts a soft AP using ap_ssid/ap_password from secrets.
 */
void setupAccessPoint();

/**
 * @brief Start HTTP server and attach routes.
 *
 * Currently opens server on port 80 and prepares for incoming POST /data.
 */
void setupHttpServer();

/**
 * @brief Global server object that listens for incoming client connections.
 * @note Defined in wifiHandler.cpp
 */
extern WiFiServer server; // Server listens to port 80

#endif // WIFIHANDLER_H