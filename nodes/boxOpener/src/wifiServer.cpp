#include "wifiServer.h"

WifiServer::WifiServer(const char* network, const char* password) {
    local_IP = IPAddress(192, 168, 2, 3);
    gateway = IPAddress(192, 168, 2, 3);
    subnet = IPAddress(255, 255, 255, 0);
    ssid = network;
    _password = password;
}

WifiServer::~WifiServer() {}

void WifiServer::setup() {
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ssid, _password);
    Serial.print("Serving Access Point on IP ");
    Serial.print(WiFi.softAPIP());
    Serial.print(" for ");
    Serial.println(ssid);
    Serial.println();

    // Bind a lambda function as the client connection callback
    server.onClient(
        [this](void* arg, AsyncClient* client) {
            this->on_client_connected(arg, client);
        },
        this);

    server.begin();
}
