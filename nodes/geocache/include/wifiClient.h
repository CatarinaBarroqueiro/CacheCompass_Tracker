#pragma once  // Safeguard

#include <WiFi.h>
#include <AsyncTCP.h>


#define SSID_AD_HOC "ad_hoc_esp32"
#define PASSWORD_AD_HOC "123456789"
#define SERVER_PORT 80
#define MTU 1500

class WifiClient 
{
protected:
    uint8_t packetCounter = 0;

    IPAddress serverIP;  // Access Point IP address

private:
    const char* _password;

    void send(uint8_t* data, size_t size);

public:
    WifiClient(const char* network, const char* password);
    ~WifiClient();

    void connect();
    void disconnect();
    void ping();

    /*! **Attention**: Function on_receive is supposed to be defined in the Src file 
     * from which is called. This means that it's not implemented in this associated 
     * cpp file. */
    virtual void on_receive(void* arg, AsyncClient* client, void* data, size_t len);

    const char* ssid;
    AsyncClient client;
};