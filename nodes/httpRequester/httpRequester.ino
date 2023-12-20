/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-http-request
 *
 * Also using Arduino_JSON, by Arduino, library to process requests
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char WIFI_SSID[] = "YOUR_WIFI_SSID";         // CHANGE IT
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD"; // CHANGE IT

String HOST_NAME   = "http://51.20.64.70:3000"; // CHANGE IT
String PATH_NAME   = "/user/id";      // CHANGE IT
//String PATH_NAME   = "/products/arduino.php";      // CHANGE IT
String queryString = "id=";

int counter = 0;

HTTPClient http;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());



  
}

void loop() {
  Serial.println("Query URL: " + HOST_NAME + PATH_NAME + "?" + queryString);

  http.begin(HOST_NAME + PATH_NAME + "?" + queryString + String(counter++));
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Received payload:");
      Serial.println(payload);

      JSONVar response = JSON.parse(payload);

      if(JSON.typeof(response) == "array"){
        /*
        // myArray.length() can be used to get the length of the array
        Serial.print("myArray.length() = ");
        Serial.println(myArray.length());
        Serial.println();
        */

        Serial.print("JSON.typeof(myArray[0]) = ");
        Serial.println(JSON.typeof(response[0]));
        
        Serial.println("Received ID: ");
        Serial.println(response[0]["idUser"]);
        Serial.printf("User is %s to access geocache", response[0]["flag"] ? "Authorized" : "Unauthorized");
        Serial.println();
      }
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  delay(10000);
}
