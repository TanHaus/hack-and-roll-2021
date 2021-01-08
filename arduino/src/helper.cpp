#include <WiFi.h>
#include <HTTPClient.h>

#include "helper.h"

void setup_wifi(const char* ssid, const char* password) {
  // connect to Wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to wifi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println();
}

void send_data(const char* payload, const char* url) {
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(payload);

#ifdef DEBUG
  Serial.print("Payload: ");
  Serial.println(payload);
  Serial.print("POST ");
  Serial.print(url);
  if (code > 0) {
    Serial.println(" success");
  } else {
    Serial.println(" fail");
  }
#endif
}

