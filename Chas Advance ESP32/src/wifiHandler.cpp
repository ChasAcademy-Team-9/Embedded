#include "wifiHandler.h"
#include "jsonParser.h"
#include "wifiHandler.h"
#include "ARDUINOSECRETS.h"

void WifiHandler::init()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nESP32 connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Define route
  server.on("/data", HTTP_POST, [&]()
            { handlePostRequest(); });

  server.begin();
  Serial.println("HTTP server started");
}

/* Function to handle POST requests to /data
    It reads the JSON body, parses it, and logs the sensor data.*/
void WifiHandler::handlePostRequest()
{
  if (server.hasArg("plain"))
  { // "plain" contains POST body
    String body = server.arg("plain");

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error)
    {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      server.send(400, "text/plain", "Bad JSON");
      return;
    }

    // Parse sensor data
    parseJson(body);
    server.send(200, "text/plain", "OK");
  }
  else
  {
    server.send(400, "text/plain", "No data received");
  }
}
