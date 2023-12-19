#include "wifiClient.h"

WifiClient::WifiClient(const char* network, const char* password) {
    ssid = network;
    _password = password;
    serverIP = IPAddress(192, 168, 2, 3);
}

WifiClient::~WifiClient() {}

void WifiClient::connect() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, _password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.printf("Attempting to connect to ad-hoc network: %s", ssid);
        Serial.println();
    }

    Serial.print("Connected to ad-hoc network. My IP address is: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    // Create a lambda function that captures 'this' and calls 'on_receive'
    //  --> because callbacks can't access non-static functions directly
    auto onDataCallback = [this](void* arg, AsyncClient* client, void* data,
                                 size_t len) {
        this->on_receive(arg, client, data, len);
    };

    // Bind the lambda function to 'client.onData'
    client.onData(onDataCallback, this);

    while (!client.connect(serverIP, SERVER_PORT)) {
        Serial.print("Attempting to connect to Server IP: ");
        Serial.print(serverIP);
        Serial.print(", Port: ");
        Serial.println(SERVER_PORT);
        delay(1000);
    }
}

void WifiClient::send(uint8_t* data, size_t size) {
    if (client.connected()) {
        client.write(reinterpret_cast<const char*>(data),
                     sizeof(data) / sizeof(data[0]));
        Serial.printf("Sending packet %d, with %d bytes to server...",
                      packetCounter, sizeof(data) / sizeof(data[0]));
        Serial.println(" ");

    } else {
        Serial.println("Client not connected to server!");
    }
}

void WifiClient::ping() {
    uint8_t message[6];

    message[0] = packetCounter;
    message[1] = 'H';
    message[2] = 'e';
    message[3] = 'l';
    message[4] = 'l';
    message[5] = 'o';

    send(message, 6);
}