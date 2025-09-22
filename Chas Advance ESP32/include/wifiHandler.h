#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "jsonParser.h"
#include <time.h>

//initialize wifi setup
void initWifi();
//Connects the ESP32 to the WiFi network
void connectToWiFi();
//Sets up the Access Point for the Arduino to connect to
void setupAccessPoint();
//Sets up the HTTP server to handle incoming requests
void setupHttpServer();
//Handles incoming POST requests to /data
void handlePostRequest();

void updateLogger();

extern unsigned long timeSinceDataReceived;
extern WebServer server; // Server listen to port 80

#endif