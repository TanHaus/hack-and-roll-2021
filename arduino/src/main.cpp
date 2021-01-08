#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Wifi credientials should be here
// it should define 'ssid' and 'password'
// e.g.
// const char* ssid = "ssid";
// const char* password = "password";
#include <wifi_cred.h>

// server settings
const char* server_ip = "192.168.1.7"; 
const int port = 3000;
// const char* url = "http://192.168.1.7:3000";

void send_data(char*, char*);

void setup() {
  Serial.begin(9600);

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
}

void loop() {
  HTTPClient http;
  http.begin("http://192.168.1.7:3000/");

  int responseCode = http.GET();
  if (responseCode > 0) {
    Serial.println(http.getString());
  }
  http.end();
  delay(2000);
}

void send_data(char* payload, char* url) {
  HTTPClient http;
  http.begin(url);
  int code = http.POST(payload);

  if (code > 0) {

  } else {

  }
}