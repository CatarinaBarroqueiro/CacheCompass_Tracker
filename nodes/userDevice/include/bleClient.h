#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

////////// DEFINES //////////
#define BLE_MTU 251
#define BLE_HEADER 3
#define BLE_PAYLOAD BLE_MTU - BLE_HEADER

class BleClient
{
private:
    /* Specify the Service UUID of Server */
    BLEUUID serviceUUID{"4fafc201-1fb5-459e-8fcc-c5c9c331914b"};
    /* Specify the Characteristic UUID of Server */
    BLEUUID charUUID{"beb5483e-36e1-4688-b7f5-ea07361b26a8"};
    boolean doConnect = false;
    boolean doScan = false;
    BLERemoteCharacteristic* pRemoteCharacteristic;
    BLEAdvertisedDevice* myDevice;
    uint packetCounter = 0x00;
    uint16_t negotiatedMTU;

    static void notify_callback(BLERemoteCharacteristic* pBLERemoteCharacteristic,uint8_t* pData, size_t length, bool isNotify);

    class MyClientCallback : public BLEClientCallbacks
    {
        BleClient* parent;  // A pointer to the parent BleClient object

    public:
        MyClientCallback(BleClient* parent) : parent(parent) {}
        void onConnect(BLEClient* pclient) {}
        void onDisconnect(BLEClient* pclient)
        {
            parent->connected = false;
            Serial.println("onDisconnect");
        }
    };

    /* Scan for BLE servers and find the first one that advertises the service we are looking for. */
    class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
    {
        BleClient* parent;  // A pointer to the parent BleClient object

    public:
        MyAdvertisedDeviceCallbacks(BleClient* parent) : parent(parent) {}
        /* Called for each advertising BLE server. */
        void onResult(BLEAdvertisedDevice advertisedDevice)
        {
            //Serial.print("BLE Advertised Device found: ");
            //Serial.println(advertisedDevice.toString().c_str());
            /* We have found a device, let us now see if it contains the service we are looking for. */
            if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(parent->serviceUUID))
            {
                BLEDevice::getScan()->stop();
                parent->myDevice = new BLEAdvertisedDevice(advertisedDevice);
                parent->doConnect = true;
                parent->doScan = true;
            }
        }
    };

    bool connect_server();

public:
    boolean connected = false;
    
    BleClient(){};
    ~BleClient(){};
    void setup();
    void send(uint8_t* data, size_t size);

};