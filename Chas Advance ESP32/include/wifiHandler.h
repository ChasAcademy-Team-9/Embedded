#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>

class WifiHandler
{
    public:
        WifiHandler():server(80) {};
        ~WifiHandler(){server.stop();};

        void init(); 
        WebServer server; // Server listen to port 80
        void handlePostRequest();
};

#endif