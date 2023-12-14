#include "bleServer.h"

BleServer::BleServer(int nodeId) {
    this->nodeId = nodeId;
}
BleServer::~BleServer() {}

/*! 
    read data from BLE, the `len` parameter is the length of the data received
    and is altered by the address of the variable provided
*/
uint8_t* BleServer::read(uint8_t* len) {
    uint8_t* value = pCharacteristic->getData();

    if (value[0] != 0x2f && connected) {
        *len = pCharacteristic->getLength();
        // Check if the value received is different from the previous
        if (memcmp(value, previousValue, *len) != 0) {
            // Update previous value
            memcpy(previousValue, value, *len);
            // Return the new value
            return value;
        }
        return nullptr;
    }

    return nullptr;
}

void BleServer::setup() {
    String deviceName = "GeoCache_" + String(nodeId);
    BLEDevice::init(deviceName.c_str());
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    pServer->setCallbacks(new MyBLEServerCallbacks(this));
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    //pCharacteristic->setMTU(512); // Set your desired MTU size here
    pCharacteristic->setValue("/");
    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(
        0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    Serial.println("BLE, Gateway visible to other ble devices!");
    Serial.println();
}