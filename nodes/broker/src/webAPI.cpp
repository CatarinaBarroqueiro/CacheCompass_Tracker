#include <webAPI.h>

WebAPI::WebAPI() {}

WebAPI::~WebAPI() {}

void WebAPI::connect_wifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.printf("Connecting to %s", WIFI_SSID);
    Serial.println();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

bool WebAPI::request_user_authorized(uint16_t userId) {
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
                Serial.printf(" - User is %s to access geocache",
                              authorized ? "Authorized" : "Unauthorized");
                Serial.println();
            }

        } else {
            // HTTP header has been send and Server response header has been handled
            Serial.printf(" - [HTTP] GET... code: %d", httpCode);
            Serial.println();
        }
    } else {
        Serial.printf(" - Error: HTTP, GET... failed: %s",
                      http.errorToString(httpCode).c_str());
        Serial.println();
    }

    http.end();

    return authorized;
}

bool WebAPI::post_discovery(uint16_t nodeId, uint16_t userId,
                            unsigned long timestamp, bool authorized) {

    String infoString = HOST_NAME + DISCOVERY_PATH_NAME;

    Serial.println(" - Posting Discovery to URL:" + infoString);

    http.begin(infoString);
    http.addHeader("Content-Type", "application/json");

    JSONVar discData;
    discData["box"] = nodeId;
    discData["user"] = userId;
    discData["discTime"] = get_time_string(timestamp);
    discData["authorized"] = authorized;

    String jsonString = JSON.stringify(discData);
    Serial.println(" - Post data: " + jsonString);
    int httpCode = http.POST(jsonString);

    bool status = false;

    // httpCode will be negative on error
    if (httpCode > 0) {
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == 201) {
            Serial.println(" - Post executed successfully ");
            status = true;
        } else {
            // HTTP header has been send and Server response header has been handled
            Serial.printf(" - [HTTP] GET... code: %d", httpCode);
            Serial.println();
        }
    } else {
        Serial.printf(" - Error: HTTP, GET... failed: %s",
                      http.errorToString(httpCode).c_str());
        Serial.println();
    }

    http.end();

    return status;
}