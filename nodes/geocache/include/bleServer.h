#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
//! Maximum Transmission Unit for BLE communication, it's 251 since esp32s
//   have Bluetooth 4.2
#define BLE_MTU 251
//! Unique identifier for the BLE service
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
//! Unique identifier for the BLE characteristic
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

/*
    ##########################################################################
    ############             BleServer declaration                ############
    ##########################################################################
*/
class BleServer {
   private:
    /*!< Pointer to the BLE server. */
    BLEServer* pServer;
    /*!< Pointer to the BLE service. */
    BLEService* pService;
    /*!< Pointer to the BLE characteristic. */
    BLECharacteristic* pCharacteristic;
    /*!< Buffer to store previous BLE value. */
    uint8_t previousValue[BLE_MTU];

    /*! \class MyBLEServerCallbacks
        \brief Callbacks for BLE server events.
    */
    class MyBLEServerCallbacks : public BLEServerCallbacks {
        /*!< pointer to the parent BleClient object. */
        BleServer* parent;

       public:
        /*! \brief Constructor for MyBLEServerCallbacks.
            \param parent A pointer to the parent BleClient object.
        */
        MyBLEServerCallbacks(BleServer* parent) : parent(parent) {}

        /*! \brief Handles connection event.
            \param pServer Pointer to the BLE server.
        */
        void onConnect(BLEServer* pServer) { parent->connected = true; }

        /*! \brief Handles disconnection event.
            \param pServer Pointer to the BLE server.
        */
        void onDisconnect(BLEServer* pServer) {
            parent->connected = false;
            BLEDevice::startAdvertising();
        }
    };

    //! Flag indicating if any client is connected to server
    bool connected = false;
    //! Id of the geocache node
    int nodeId = 1;

   public:
    /*! \brief Constructor for BleServer class.
        \param nodeId The ID of the node.
    */
    BleServer(int nodeId);

    /*! \brief Destructor for BleServer class.
    */
    ~BleServer();

    /*! \brief Set up the BLE server and characteristics.
    */
    void setup();

    /*! \brief Read data from BLE.
        \param len Pointer to the length of the data received, modified by the function.
        \return A pointer to the new value `if it differs` from the previous value; otherwise, nullptr.
    */
    uint8_t* read(uint8_t* len);
};