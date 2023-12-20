#pragma once

//! \attention for WiFi include to always be first
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
//! SSID of your internet enabled WiFi network
#define WIFI_SSID "YOUR_WIFI_SSID";  // CHANGE IT
//! Password of your internet enabled WiFi network
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD";  // CHANGE IT
//! URL or Host name of the API service
#define HOST_NAME "http://51.20.64.70:3000"
//! Path of the request of user by Id
#define USER_ID_PATH_NAME "/user/id"
//! Query variables of the request of user by Id
#define USER_ID_QUERY_VARS "id="

/*
    ##########################################################################
    ############               webAPI declaration                 ############
    ##########################################################################
*/
class webAPI {
   private:
    HTTPClient http;

   public:
    webAPI();
    ~webAPI();

    void connect_wifi();

    bool request_user_authorized(uint16_t userId);

    bool post_discovery(uint16_t nodeId, uint16_t userId,
                        unsigned long timestamp);
};
