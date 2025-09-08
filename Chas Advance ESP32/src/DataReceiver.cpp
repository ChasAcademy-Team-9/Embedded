#include "DataReceiver.h"
#include <ArduinoJson.h>
#include "jsonParser.h"

/* Function to handle POST requests to /data
    It reads the JSON body, parses it, and logs the sensor data.
*/
void HandlePostRequest(WebServer &server)
{
    
  if (server.hasArg("plain")) { // "plain" contains POST body
    String body = server.arg("plain");
    
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    
    if (error) {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      server.send(400, "text/plain", "Bad JSON");
      return;
    }

    // Parse sensor data
    parseJson(body);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "No data received");
  }
}