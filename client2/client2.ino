#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTTYPE DHT22

uint8_t DHTPin = 5; // pin D1
int photoresistorPin = 4; // pin D2

DHT dht(DHTPin, DHTTYPE);


String token;

float temperature, humidity;

const char* ssidList[] = {"Mihăiță_Net", "Mihăiță_Net2", "Mihaita_Net_Mi8"};         // The SSID (name) of the Wi-Fi network you want to connect to
const char* passwordList[] = {"mihai2001", "mihai2000", "mihai_daian"};        // The password of the Wi-Fi network

void connectToWifi();
void sendRegisterRequest();
void sendDHTSensorData();
void sendAuthenticateRequest();

void setup() {
  Serial.begin(115200);
  pinMode(DHTPin, INPUT);
  pinMode(photoresistorPin, INPUT);
  dht.begin();
  connectToWifi();
  sendAuthenticateRequest();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }

  delay(2.5 * 60 * 1000);
  sendDHTSensorData_Temperature();
  delay(2.5 * 60 * 1000);
  sendDHTSensorData_Humidity();
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

    int connectionTimeout = 10; // Timeout in seconds
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

void sendAuthenticateRequest() {
  WiFiClient client;  // Create a WiFiClient object to use with HTTPClient

  HTTPClient http;
  String email = "mdaian150@yahoo.com";
  String password = "1234";

  // Create the JSON payload
  String payload = "{";
  payload += "\"email\": \"" + email + "\",";
  payload += "\"password\": \"" + password + "\"";
  payload += "}";

  http.begin(client, "http://mihaiddomain150.go.ro:8080/iotify/auth/authenticate");
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(payload);
  delay(2000);
  if (httpCode > 0) {
    String response = http.getString();
    Serial.println(httpCode);
    Serial.println(response);
    if (response.indexOf("token") != -1) {
      int tokenStart = response.indexOf(":") + 2;
      int tokenEnd = response.indexOf("\"", tokenStart);
      token = response.substring(tokenStart, tokenEnd);
    }
    Serial.print("Token: ");
    Serial.println(token);
  } else {
    Serial.println("Error sending the request ");
    Serial.println(httpCode);
  }

  
  http.end();
}

void sendDHTSensorData_Temperature() {

  temperature = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);

  if (-40 < temperature && temperature < 80) {
      // Create the JSON payload for temperature
    String payload = "{";
    payload += "\"value\": " + String(temperature);
    payload += "}";

    WiFiClient client;
    HTTPClient http;

    // Set the target URL
    http.begin(client, "http://mihaiddomain150.go.ro:8080/iotify/Temperature/DHT22/save");
    // Add headers
    http.addHeader("Content-Type", "application/json");
    Serial.println("Bearer " + token);
    http.addHeader("Authorization", "Bearer " + token);

    // Make the POST request
    int httpCode = http.POST(payload);

    // Check the response
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error sending the request DHT");
    }
  }
}

void sendDHTSensorData_Humidity() {

  humidity = dht.readHumidity();
  Serial.print(", Humidity: ");
  Serial.println(humidity);

  if (humidity >= 0 && humidity <= 100) {
    // Create the JSON payload for humidity
    String payload = "{";
    payload += "\"value\": " + String(humidity);
    payload += "}";

    WiFiClient client;
    HTTPClient http;

    // Set the target URL
    http.begin(client, "http://mihaiddomain150.go.ro:8080/iotify/Humidity/DHT22/save");

    // Add headers
    http.addHeader("Content-Type", "application/json");
    Serial.println("Bearer " + token);
    http.addHeader("Authorization", "Bearer " + token);

    // Make the POST request
    int httpCode = http.POST(payload);

    // Check the response
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error sending the request DHT");
    }
    // End the request
    http.end();
  }
}
