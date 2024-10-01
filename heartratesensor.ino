#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DFRobot_Heartrate.h"

const char* ssid = "Redmi Note 7 Pro";
const char* password = "cosmosapiens";

const char* serverUrl = "http://13.201.118.241/heart.php"; // Replace with your server URL

#define heartratePin A0
#define LED_PIN 2

DFRobot_Heartrate heartrate(DIGITAL_MODE);

unsigned long previousMillis = 0;
const long interval = 120000; // 2 minutes interval

// Add variables for the additional data to be sent
String age = "25"; // Example age
String gender = "Male"; // Example gender
String designation = "Engineer"; // Example designation
String module = "Health Monitoring"; // Example module

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  uint8_t rateValue;
  heartrate.getValue(heartratePin);
  rateValue = heartrate.getRate();
  if (rateValue) {
    Serial.println("Heart Rate: " + String(rateValue));
    digitalWrite(LED_PIN, HIGH);
  }

  if (rateValue) {
    sendData(rateValue);
    digitalWrite(LED_PIN, LOW);
  }

  delay(500);
}

void sendData(uint8_t rateValue) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    Serial.println("Starting connection to server...");

    if (http.begin(client, serverUrl)) {
      Serial.println("Connected to server");

      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Include additional data in the POST request
      String postData = "rate=" + String(rateValue) + 
                        "&age=" + age + 
                        "&gender=" + gender + 
                        "&designation=" + designation + 
                        "&module=" + module;
      Serial.println("Sending data: " + postData);

      int httpResponseCode = http.POST(postData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print("Server response: ");
        Serial.println(response);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("Unable to connect to server");
    }
  } else {
    Serial.println("Error in WiFi connection");
  }
}
