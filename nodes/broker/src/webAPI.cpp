#include <webAPI.h>

webAPI::webAPI(/* args */) {}

webAPI::~webAPI() {}

void webAPI::connect_wifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Connecting to " + WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

bool webAPI::request_user_authorized(uint16_t userId) {
    Serial.println("Requesting user authorization...");

    String infoString = HOST_NAME + USER_ID_PATH_NAME + "?" +
                        USER_ID_QUERY_VARS + String(userId);

    Serial.println(" - Query URL: " + infoString);

    http.begin(infoString);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.GET();
    bool authorized = false;

    // httpCode will be negative on error
    if (httpCode > 0) {
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(" - Received payload:" + payload);

            JSONVar response = JSON.parse(payload);

            if (JSON.typeof(response) == "array") {
                /*
            
                Serial.print("response.length() = ");
                Serial.println(response.length());
                Serial.println();
                
                Serial.print("JSON.typeof(myArray[0]) = ");
                Serial.println(JSON.typeof(response[0]));

                Serial.println("Received ID: ");
                Serial.println(response[0]["idUser"]);
                */

                authorized = response[0]["flag"];
                Serial.printf("User is %s to access geocache",
                              authorized ? "Authorized" : "Unauthorized");
                Serial.println();
            }

        } else {
            // HTTP header has been send and Server response header has been handled
            Serial.printf(" - [HTTP] GET... code: %d\n", httpCode);
        }
    } else {
        Serial.printf(" - Error: HTTP, GET... failed: %s\n",
                      http.errorToString(httpCode).c_str());
    }

    http.end();

    return authorized;
}

bool webAPI::post_discovery(uint16_t nodeId, uint16_t userId,
                            unsigned long timestamp) {}