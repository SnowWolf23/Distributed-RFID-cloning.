#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Realme 2";
const char* password = "2001debuRoy07";

const char* serverUrl = "https://esp-server-omc7.onrender.com/get_serial_data";
String g;
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
    WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  while (!Serial) {
    ; // Wait for serial port to connect
  }

  // Read user input from the serial monitor
  Serial.println("Enter a number to send to the server:");
  while (Serial.available() == 0) {
    // Wait for user input
  }
  String data = Serial.readStringUntil('\n');
  data.trim();
  sendHttpPostRequest(data);
  delay(5000);
  
}
void loop() {
  Serial2.println(g);
}
void sendHttpPostRequest(String numberToSend) {
  HTTPClient http;

  // Construct the URL with the number as a query parameter
  String url = serverUrl;
  url += "?serial=" + numberToSend;

  http.begin(url);

  // Send the HTTP POST request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    response=response.substring(0,32);
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    g=response;
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}
