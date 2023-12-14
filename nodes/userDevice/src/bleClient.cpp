#include <bleClient.h>

void BleClient::notify_callback(
    BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData,
    size_t length, bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

/* Start connection to the BLE Server */
bool BleClient::connect_server() {
    Serial.println("Starting BLE Client");
    if (myDevice == nullptr)
        Serial.println("Device not found");
    Serial.printf("Forming a connection to %s",
                  myDevice->getAddress().toString().c_str());
    Serial.println();

    BLEClient* pClient = BLEDevice::createClient();
    Serial.println(" - Created client");
    pClient->setClientCallbacks(new MyClientCallback(this));
    /* Connect to the remote BLE Server */
    pClient->connect(
        myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    pClient->setMTU(
        BLE_MTU);  //set client to request maximum MTU from server (default is 23 otherwise) (this can return a bool on status)
    negotiatedMTU = pClient->getMTU();
    Serial.printf(" - Agreed on MTU of %d bytes", negotiatedMTU);
    Serial.println();

    /* Obtain a reference to the service we are after in the remote BLE server */
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our service");
    /* Obtain a reference to the characteristic in the service of the remote BLE server */
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(charUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our characteristic");
    /* Read the value of the characteristic */
    if (pRemoteCharacteristic->canRead()) {
        std::string value = pRemoteCharacteristic->readValue();
        Serial.print("The characteristic value was: ");
        Serial.println(value.c_str());
    }
    if (pRemoteCharacteristic->canNotify()) {
        pRemoteCharacteristic->registerForNotify(notify_callback);
    }
    connected = true;
    return true;
}

void BleClient::send(uint8_t* data, size_t size) {
    if (connected) {
        pRemoteCharacteristic->writeValue(data, size);

        //printHEXPacket(message, 33); -- debugging
        Serial.printf("Sending %d bytes to server...", size);
        Serial.println(" ");
    } else {
        Serial.println("Client not connected to server!");
    }
}

void BleClient::setup() {
    //setup ble
    Serial.println("Starting BLE Client application...");

    BLEDevice::init("userDevice");
    /* Retrieve a Scanner and set the callback we want to use to be informed when we
        have detected a new device.  Specify that we want active scanning and start the
        scan to run for 5 seconds. */
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(
        new MyAdvertisedDeviceCallbacks(this));
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);

    while (!doConnect) {
        Serial.println("Searching for GeoCache device...");
        delay(1000);
    }

    while (!connect_to_server()) {
        BLEDevice::getScan()->start(
            0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
        Serial.println("Attempting to connect to the BLE Server...");
        delay(1000);
    }
    connected = true;
    Serial.println("Connected to the BLE Server");
}