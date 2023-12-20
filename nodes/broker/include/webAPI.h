#pragma once

//! \attention for WiFi include to always be first
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <message.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
//! SSID of your internet enabled WiFi network
const char WIFI_SSID[] = "YOUR_WIFI_SSID";  // CHANGE IT
//! Password of your internet enabled WiFi network
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";  // CHANGE IT
//! URL or Host name of the API service
const String HOST_NAME = "http://51.20.64.70:3000";
//! Path of the request of user by Id
const String USER_ID_PATH_NAME = "/user/id";
//! Query variables of the request of user by Id
const String USER_ID_QUERY_VARS = "id=";
//! Path of the request of discovery
const String DISCOVERY_PATH_NAME = "/discovery";

/*
    ##########################################################################
    ############               WebAPI declaration                 ############
    ##########################################################################
*/
/*! \class WebAPI
    \brief Handles interactions with the web API.
*/
class WebAPI {
   private:
    //!< HTTPClient instance for making HTTP requests.
    HTTPClient http;

   public:
    //! \brief Constructor for WebAPI class.
    WebAPI();

    //! \brief Destructor for WebAPI class.
    ~WebAPI();

    /*! \brief Connect to the WiFi network.
    */
    void connect_wifi();

    /*! \brief Request user authorization by user ID.
        \param userId ID of the user to request authorization for.
        \return True if authorized, false otherwise.
    */
    bool request_user_authorized(uint16_t userId);

    /*! \brief Post discovery information to the API.
        \param nodeId ID of the node.
        \param userId ID of the user.
        \param timestamp Timestamp of the discovery.
        \param authorized Authorization status.
        \return True if post is successful, false otherwise.
    */
    bool post_discovery(uint16_t nodeId, uint16_t userId,
                        unsigned long timestamp, bool authorized);
};
