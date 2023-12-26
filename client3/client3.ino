#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const int RELAY_PIN = 5;
bool isRelayOn = false;

String token;

const char* ssidList[] = { "Mihăiță_Net", "Mihăiță_Net2", "Mihaita_Net_Mi8" };  // The SSID (name) of the Wi-Fi network you want to connect to
const char* passwordList[] = { "mihai2001", "mihai2000", "mihai_daian" };       // The password of the Wi-Fi network

void connectToWifi();

void setup() {
  Serial.begin(115200);
  connectToWifi();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }
  waitSignal();
}

void connectToWifi() {
  delay(1000);
  Serial.println('\n');

  int ssidCount = sizeof(ssidList) / sizeof(ssidList[0]);
  for (int i = 0; i < ssidCount; i++) {
    const char* ssid = ssidList[i];
    const char* password = passwordList[i];

    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println(" ...");

    int connectionTimeout = 10;  // Timeout in seconds
    int connectionTimer = 0;
    while (WiFi.status() != WL_CONNECTED && connectionTimer < connectionTimeout) {
      delay(1000);
      connectionTimer++;
      Serial.print(connectionTimer);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println('\n');
      Serial.println("Connection established!");
      Serial.print("Connected to: ");
      Serial.println(ssid);
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP());
      break;
    } else {
      Serial.println('\n');
      Serial.print("Failed to connect to ");
      Serial.println(ssid);
    }
  }
}


void waitSignal() {
  WiFiClient client;
  HTTPClient http;

  String relay_url = "http://localhost:8080/iotify/controller/relay/get-state";

  http.begin(client, relay_url);
  int httpCode = http.GET();

  if(httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      if (doc["value"] != isRelayOn) {
        if (doc["value"] == true) {
          digitalWrite(RELAY_PIN, HIGH);
        } else {
          digitalWrite(RELAY_PIN, LOW);
        }
      }
    }
  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}
