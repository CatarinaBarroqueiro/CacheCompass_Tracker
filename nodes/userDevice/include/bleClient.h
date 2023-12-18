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
//! BLE communication sometimes requires 3 extra bytes to be explicitly added, that
//   are filled by the physical layer
#define BLE_HEADER 3
#define BLE_PAYLOAD BLE_MTU - BLE_HEADER
//! Unique identifier for the BLE Server service
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
//! Unique identifier for the BLE Server characteristic
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

/*
    ##########################################################################
    ############             BleClient declaration                ############
    ##########################################################################
*/
class BleClient {
   private:
    /*!< Service UUID of the server. */
    BLEUUID serviceUUID{SERVICE_UUID};
    /*!< Characteristic UUID of the server. */
    BLEUUID charUUID{CHARACTERISTIC_UUID};
    /*!< Flag for connection status. */
    boolean doConnect = false;
    /*!< Flag for scanning status. */
    boolean doScan = false;
    /*!< Pointer to the remote BLE characteristic of the server */
    BLERemoteCharacteristic* pRemoteCharacteristic;
    /*!< Pointer to the BLE advertised device. */
    BLEAdvertisedDevice* myDevice;
    uint packetCounter = 0;
    uint16_t negotiatedMTU;

    /*! \brief Callback function for BLE notifications.
        \param pBLERemoteCharacteristic Pointer to the remote BLE characteristic.
        \param pData Pointer to the data received.
        \param length Length of the data received.
        \param isNotify Indicates if it's a notification.
    */
    static void notify_callback(
        BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData,
        size_t length, bool isNotify);

    /*! \class MyClientCallback
        \brief Callbacks for BLE client events.
    */
    class MyClientCallback : public BLEClientCallbacks {
        /*!< A pointer to the parent BleClient object. */
        BleClient* parent;

       public:
        /*! \brief Constructor for MyClientCallback.
            \param parent A pointer to the parent BleClient object.
        */
        MyClientCallback(BleClient* parent) : parent(parent) {}

        /*! \brief Handles connection event.
            \param pclient Pointer to the BLE client.
        */
        void onConnect(BLEClient* pclient) {}

        /*! \brief Handles disconnection event.
            \param pclient Pointer to the BLE client.
        */
        void onDisconnect(BLEClient* pclient) {
            parent->connected = false;
            Serial.println("onDisconnect");
        }
    };

    /*! \class MyAdvertisedDeviceCallbacks
        \brief Callbacks for advertised device detection during scanning.
    */
    class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
        /*!< A pointer to the parent BleClient object. */
        BleClient* parent;

       public:
        /*! \brief Constructor for MyAdvertisedDeviceCallbacks.
            \param parent A pointer to the parent BleClient object.
        */
        MyAdvertisedDeviceCallbacks(BleClient* parent) : parent(parent) {}

        /*! \brief Called for each advertising BLE server.
            \param advertisedDevice The advertised BLE device.
        */
        void onResult(BLEAdvertisedDevice advertisedDevice) {
            //Serial.print("BLE Advertised Device found: ");
            //Serial.println(advertisedDevice.toString().c_str());
            /* We have found a device, let us now see if it contains the service we are looking for. */
            if (advertisedDevice.haveServiceUUID() &&
                advertisedDevice.isAdvertisingService(parent->serviceUUID)) {
                BLEDevice::getScan()->stop();
                parent->myDevice = new BLEAdvertisedDevice(advertisedDevice);
                parent->doConnect = true;
                parent->doScan = true;
            }
        }
    };

    /*! \brief Establishes connection to the BLE server.
        \return True if connection is successful, false otherwise.
    */
    bool connect_server();

   public:
    BleClient(){};
    ~BleClient(){};

    /*!< Flag indicating connection status. */
    boolean connected = false;

    /*! \brief Set up the BLE client.
    */
    void setup();

    /*! \brief Sends data via BLE.
        \param data Pointer to the data to be sent.
        \param size Size of the data to be sent.
    */
    void send(uint8_t* data, size_t size);

    /*! \brief Get the count of packets sent.
        \return The count of packets sent.
    */
    uint get_packet_count();
};