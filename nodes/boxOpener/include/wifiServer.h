#pragma once  // Safeguard

#include <AsyncTCP.h>
#include <WiFi.h>

#define SSID_AD_HOC "ad_hoc_esp32_mg"  //"ad_hoc_esp32"
#define PASSWORD_AD_HOC "123456789"
#define SERVER_PORT 80
#define MTU 1500

class WifiServer {
   protected:
    uint8_t packetCounter = {0x00};

    AsyncServer server = AsyncServer(SERVER_PORT);
    IPAddress local_IP;  // New associated IP address
    IPAddress gateway;   // Default Gateway
    IPAddress subnet;    // Network Mask
   private:
    const char* _password;

   public:
    WifiServer(const char* network, const char* password);
    ~WifiServer();

    void close_connections();
    void setup();

    /*! **Attention**: Function on_client_connected is supposed to be defined in the 
     * Src file from which is called. This means that it's not implemented in this 
     * associated cpp file. */
    virtual void on_client_connected(void* arg, AsyncClient* client);

    const char* ssid;
};