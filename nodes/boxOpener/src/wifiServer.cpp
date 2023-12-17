#include "wifiServer.h"

WifiServer::WifiServer(const char* network, const char* password) {
    local_IP = IPAddress(192, 168, 2, 3);
    gateway = IPAddress(192, 168, 2, 3);
    subnet = IPAddress(255, 255, 255, 0);
    ssid = network;
    _password = password;
}

WifiServer::~WifiServer() {
    close_connections();
}

void WifiServer::setup() {
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ssid, _password);
    //IPAddress IP = WiFi.softAPIP();
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

    // There are other functions that can be bound, like disconnected

    server.begin();
}

void WifiServer::close_connections() {
    // will probably need a list for this
}
