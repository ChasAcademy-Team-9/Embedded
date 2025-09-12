#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

class WifiHandler
{
public:
    WifiHandler(): server(80) {} // Initialize server to listen on port 80;
    ~WifiHandler() { server.stop(); };

    void init();
    void handlePostRequest();
    static String getTimeStamp();

    unsigned long timeSinceDataReceived = 0;
    WebServer server; // Server listen to port 80

    static const char *ntpServer;
    static const long gmtOffset_sec;     // e.g. GMT+1
    static const int daylightOffset_sec; // add DST if needed


};

#endif