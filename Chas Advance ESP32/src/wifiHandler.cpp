/**
 * @file wifiHandler.cpp
 * @brief Implementation of WiFi, HTTP ingestion and batch processing helpers.
 *
 * This module provides:
 *  - STA + AP setup
 */

#include "wifiHandler.h"
#include "ESPSECRETS.h"

/**
 * @brief Global HTTP server instance (listening socket).
 * Constructed here and declared in header as extern.
 */
WiFiServer server;

/**
 * @brief Initialize WiFi and HTTP subsystems and configure NTP.
 *
 * Calls:
 *  - connectToWiFi() to bring up STA interface,
 *  - setupAccessPoint() to start softAP,
 *  - setupHttpServer() to start the HTTP listener,
 *  - configTime() to set NTP server and timezone offsets.
 */
void initWifi()
{
  connectToWiFi();
  setupAccessPoint();
  setupHttpServer();

  // Configure NTP client/time library
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

/**
 * @brief Connect to WiFi STA network using credentials from ESPSECRETS.
 *
 * Blocks until connected. Retries indefinitely with a short backoff between
 * attempts. Prints status to Serial on each try.
 */
void connectToWiFi()
{
  WiFi.mode(WIFI_AP_STA);
  Serial.printf("Connecting to WiFi SSID '%s'\n", sta_ssid);

  // Loop until connected
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect");
    WiFi.begin(sta_ssid, sta_password);

    // Wait for a single attempt to either succeed or timeout
    const unsigned long ATTEMPT_TIMEOUT_MS = 10000UL; // 10s per attempt
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < ATTEMPT_TIMEOUT_MS)
    {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
      break;

    // Attempt failed — wait a bit and retry
    Serial.println("\nWifi connection attempt failed, retrying in 5s...");
    delay(5000);
  }

  Serial.println("\nESP32 connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * @brief Start a soft Access Point so local devices can connect directly.
 *
 * Uses ap_ssid and ap_password from secrets.
 */
void setupAccessPoint()
{
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("ESP32 AP started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

/**
 * @brief Start the HTTP server and make it listen on port 80.
 *
 * This function currently only starts the server. Route registration occurs
 * in handleClientAsync() which reads incoming connections from server.available().
 */
void setupHttpServer()
{
  server.begin(80);
  Serial.println("HTTP server started");
}
