#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Realme 2";
const char* password = "2001debuRoy07";

const char* serverUrl = "https://esp-server-omc7.onrender.com/save_data"; // Replace with your domain and endpoint


void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  if (Serial2.available() > 0) {
    String data1 = Serial2.readStringUntil('\n');
    Serial.println("Received data from Arduino1: " + data1);
    String data2 = Serial2.readStringUntil('\n');
    Serial.println("Received data from Arduino2: " + data2);
    
    sendHttpPostRequest(data1, data2);
    Serial2.println("Gung ho , the data has been transferred");
  }
}

void sendHttpPostRequest(String string1, String string2) {
  HTTPClient http;

  // Construct the URL with parameters
  String url = serverUrl;
  url += "?uid=" + string1;
  url += "&data=" + string2;
  
  url.replace("\r", "");
  url.replace("\n", "");

  Serial.println(url);

  http.begin(url);

  // Send the HTTP POST request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}
