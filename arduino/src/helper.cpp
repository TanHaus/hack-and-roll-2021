#include <WiFi.h>
#include <HTTPClient.h>

#include "helper.h"

void setup_wifi(const char* ssid, const char* password) {
  // connect to Wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  pinMode(2, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    // delay(1000);
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
    Serial.print(".");
  }
  digitalWrite(2, HIGH);

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

