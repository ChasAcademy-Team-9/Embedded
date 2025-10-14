#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "jsonParser.h"
#include "log.h"
#include <time.h>

// initialize wifi setup
void initWifi();
// Connects the ESP32 to the WiFi network
void connectToWiFi();
// Sets up the Access Point for the Arduino to connect to
void setupAccessPoint();
// Sets up the HTTP server to handle incoming requests
void setupHttpServer();
// Handles incoming POST requests to /data
void handleClientAsync();
void processBatches(void *parameter);
// POST request helpers
bool isValidPostRequest(WiFiClient &client, const String &requestLine);
int readContentLength(WiFiClient &client);
bool readRequestBody(WiFiClient &client, std::vector<uint8_t> &buffer, int contentLength);
bool parseBatch(const std::vector<uint8_t> &buffer, uint32_t &sendMillis, std::vector<SensorData> &batch);
void respond(WiFiClient &client, int code); 

// Retry sending saved batches
void trySendPendingBatches();
bool sendJsonToServer(const String &jsonString, int batchId);
bool postBatchToServer(const std::vector<SensorData> &batch, int batchID);

extern unsigned long timeSinceDataReceived;
extern WiFiServer server; // Server listen to port 80

#endif