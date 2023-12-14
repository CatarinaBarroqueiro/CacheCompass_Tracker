#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
#define BLE_MTU 251
#define SERVICE_UUID \
    "4fafc201-1fb5-459e-8fcc-c5c9c331914b"  // Generate new UUIDs!
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

/*
    ##########################################################################
    ############             BleServer declaration                ############
    ##########################################################################
*/
class BleServer {
   private:
    BLEServer* pServer;
    BLEService* pService;
    BLECharacteristic* pCharacteristic;
    uint8_t previousValue[BLE_MTU];

    class MyBLEServerCallbacks : public BLEServerCallbacks {
        BleServer* parent;  // A pointer to the parent BleClient object

       public:
        MyBLEServerCallbacks(BleServer* parent) : parent(parent) {}
        void onConnect(BLEServer* pServer) { parent->connected = true; }
        void onDisconnect(BLEServer* pServer) {
            parent->connected = false;
            BLEDevice::startAdvertising();
        }
    };

    bool connected = false;
    int nodeId = 1;

   public:
    BleServer(int nodeId);
    ~BleServer();

    void setup();

    uint8_t* read(uint8_t* len);
};