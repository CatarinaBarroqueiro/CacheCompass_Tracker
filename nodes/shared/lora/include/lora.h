#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <SX127XLT.h>
#include <message.h>
#include "driver/rtc_io.h"

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
//! Select pin on SPI interface
#define NSS 12
//! Reset pin on SPI Interface
#define NRESET 15
//! SCK or clock pin on SPI interface
#define SCK 4
//! MISO or data to master pin on SPI interface
#define MISO 13
//! MOSI or data from master pin on SPI interface
#define MOSI 2
//! pin number for ESP32CAM on board red LED, set logic level low for on
#define REDLED 33
//! Name of the device used
#define LORA_DEVICE DEVICE_SX1278
/*! Although 255 is the maximum packet size, 222 is the maximum size allowed
      by LoraWan (+13 bytes overhead) - https://avbentem.github.io/airtime-calculator/ttn/eu868 */
#define LORA_PAYLOAD 222

/*
    ##########################################################################
    ############                    Logger                        ############
    ##########################################################################
*/
enum DEBUG_MODE { VERBOSE, SILENT };

/*
    ##########################################################################
    ############              LoRa868 declaration                 ############
    ##########################################################################
*/
class LoRa868 {
   private:
    /*
        ######################################################################
        ############         Start LoRa modem parameters          ############
        ######################################################################
    */
    // Transmission Frequency - 868 MHz
    const uint32_t Frequency = 868000000;
    // Offset frequency for calibration purposes
    const uint32_t Offset = 0;
    // LoRa Bandwidth
    const uint8_t Bandwidth = LORA_BW_500;
    // LoRa spreading factor - SF7 is lowest, which means more data rate, but
    //  less range and more prone to errors. SF12 is highest and more robust
    const uint8_t SpreadingFactor = LORA_SF7;
    // LoRa coding rate
    const uint8_t CodeRate = LORA_CR_4_5;
    // low data rate optimisation setting
    const uint8_t Optimisation = LDRO_AUTO;
    // LoRa Tx power
    const int8_t TxPower = 10;
    // Timeout on Received, to wait for a packet
    const uint32_t RxTimeoutMs = 5000;
    // Timeout on Transmitter, to wait for a packet
    const uint32_t TxTimeoutMs = 2500;
    /*
        ######################################################################
        ############          End LoRa modem parameters           ############
        ######################################################################
    */

    bool configDone = false;
    bool terminated = false;
    int TxPacketCount = 1;
    int RxPacketCount = 1;
    DEBUG_MODE mode = VERBOSE;

    /*!
    Method to handle packet errors in the LoRa communication
      \param packetRSSI RSSI value of the received packet
      \param packetSNR SNR value of the received packet
    */
    void packet_is_error(int16_t packetRSSI, int8_t packetSNR);

    /* Duty Cycle Controller variables */
    unsigned long lastTransmissionTime = 0;
    unsigned long dutyCycleTime = 0;  // seconds
    bool dutyCycleEnabled = true;

    // LoRa instance object
    SX127XLT LoRa;

    uint16_t nodeId;

   public:
    /*!
    Constructor for LoRa868 class
      \param nodeId Node ID for the LoRa device
      \param applyDutyCycle Flag to apply duty cycle control (default: true)
    */
    LoRa868(uint16_t nodeId, bool applyDutyCycle = true);

    //! Destructor for LoRa868 class
    ~LoRa868();

    /*!
    Configure the LoRa868 class with specified debugging mode
      \param mode Debugging mode (VERBOSE or SILENT)
      \return Boolean indicating success or failure of configuration
    */
    bool configure(DEBUG_MODE mode);

    /*!
    Check the connection status of the LoRa868 module
      \return Boolean indicating the connection status
    */
    bool connected();

    /*!
    Send data through the LoRa868 module
      \param buffer Pointer to the data to be sent
      \param size Size of the data to be sent
      \return Boolean indicating success or failure of transmission
    */
    bool send(uint8_t* buffer, size_t size);

    /*!
    Terminate the LoRa868 module
      \return Boolean indicating success or failure of termination
    */
    bool terminate();

    /*!
    Receive data through the LoRa868 module
      \param buffer Pointer to store received data
      \return Size of the received packet
    */
    uint8_t receive(uint8_t* buffer);

    /*!
    Get the status of configuration completion
      \return Boolean indicating whether configuration is done or not
    */
    bool get_configDone();

    /*!
    Get the count of transmitted packets
      \return Count of transmitted packets
    */
    int get_tx_packet_count();
};