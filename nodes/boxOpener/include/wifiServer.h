#pragma once  // Safeguard

//! \attention for WiFi include to always be first
#include <WiFi.h>
#include <AsyncTCP.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
//! Default SSID for AD_HOC network
#define SSID_AD_HOC "ad_hoc_esp32"
//! Default Password for AD_HOC network
#define PASSWORD_AD_HOC "123456789"
//! Server port for the server to be bounded
#define SERVER_PORT 80
//! Maximum Transmission Unit for communication
#define MTU 1500

/*
    ##########################################################################
    ############             WifiServer declaration               ############
    ##########################################################################
*/
/*! \class WifiServer
    \brief Handles Wi-Fi server functionalities.
*/
class WifiServer {
   protected:
    /*!< Counter for packets. */
    uint8_t packetCounter = 0;

    /*!< Async server instance. */
    AsyncServer server = AsyncServer(SERVER_PORT);
    /*!< New associated IP address. */
    IPAddress local_IP;
    /*!< Default Gateway. */
    IPAddress gateway;
    /*!< Network Mask. */
    IPAddress subnet;

   private:
    /*!< Pointer to the password. */
    const char* _password;

   public:
    /*! \brief Constructor for WifiServer class.
        \param network Pointer to the SSID of the network.
        \param password Pointer to the password of the network.
    */
    WifiServer(const char* network, const char* password);

    /*! \brief Destructor for WifiServer class.
    */
    ~WifiServer();

    /*! \brief Set up the Wi-Fi server.
    */
    void setup();

    /*! \brief Callback for handling client connections.
        \param arg Pointer to additional data.
        \param client Pointer to the connected AsyncClient.
        \attention Function on_client_connected is supposed to be defined in the 
            Src file from which is called. This means that it's not implemented in this 
            associated cpp file
    */
    virtual void on_client_connected(void* arg, AsyncClient* client);

    /*!< Pointer to the SSID. */
    const char* ssid;
};