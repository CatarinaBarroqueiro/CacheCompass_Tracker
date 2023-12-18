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
/*! \class WifiClient
    \brief Handles Wi-Fi client functionalities.
*/
class WifiClient {
   protected:
    /*!< Counter for packets. */
    uint8_t packetCounter = 0;
    /*!< IP address of the access point. */
    IPAddress serverIP;

   private:
    /*!< Pointer to the password. */
    const char* _password;

    /*! \brief Send data over the connection.
        \param data Pointer to the data to be sent.
        \param size Size of the data to be sent.
    */
    void send(uint8_t* data, size_t size);

   public:
    /*! \brief Constructor for WifiClient class.
        \param network Pointer to the SSID of the network.
        \param password Pointer to the password of the network.
    */
    WifiClient(const char* network, const char* password);
    /*! \brief Destructor for WifiClient class.
    */
    ~WifiClient();

    /*! \brief Connect to the Wi-Fi network.
    */
    void connect();

    /*! \brief Send a ping message.
    */
    void ping();

    /*! \brief Callback for receiving data.
        \param arg Pointer to additional data.
        \param client Pointer to the connected AsyncClient.
        \param data Pointer to the received data.
        \param len Length of the received data.
        \attention Function on_client_connected is supposed to be defined in the 
            Src file from which is called. This means that it's not implemented in this 
            associated cpp file
    */
    virtual void on_receive(void* arg, AsyncClient* client, void* data,
                            size_t len);

    /*!< Pointer to the SSID. */
    const char* ssid;
    /*!< AsyncClient instance. */
    AsyncClient client;
};