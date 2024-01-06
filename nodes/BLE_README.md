![image](https://github.com/CatarinaBarroqueiro/CacheCompass_Tracker/assets/72763845/e9e85ffa-a6f8-47e3-96c6-9a82e558fce5)# BLE Concepts

## Why BLE?

In geocaching is very important for the people to be able to guide themselves with some mechanism helping through the path. Normally the technology chosen for this goal is GPS. However, in certain locations the GPS signal might be weak or inexistent, leading to more difficulties on finding the stashes by the players.
Bluetooth Low Energy (BLE) comes in clutch in this situation, since it can be used between two devices with it, meaning we can use it to create a communication between the cache and the user's phone. This way, when the user gets close enough of the BLE range, he can be guided to the cache, without having to worry about GPS functioning.

Another problem with the usual geocaching is that bad actors can access the geocache, removing the items from it and not placing anything for the next people to find.
With our idea, this can be solved. Only when the person is really close to the cache, and confirms that he has found it, the cache opens. By clicking the confirmation, this person's information will be sent from the it's BLE device to the BLE server, that will after communicate by LoRa with the main website, in order to review the information sent and classify the person as authorized, or not, to open the cache.
This way, people not using the app or that are non-authorized, can not take what's inside the cache. 



## Concepts

### BLE Server and Client

In a BLE ecosystem, there are two roles: the server and the client. An ESP32 can function as both. The server advertises its presence to potential clients and holds data that can be read by the latter. Upon detecting a server, the client can form a connection and begin receiving data, enabling point-to-point communication.

Other communication modes include:

- Broadcast mode: where a server sends data to multiple clients.
- Mesh network: a complex system where multiple devices are interlinked, allowing many-to-many connections.

### GATT

GATT (Generic Attributes) outlines how BLE devices exchange data through a hierarchical structure. This architecture comprises profiles, services, characteristics, and descriptors, with each layer possessing a unique UUID (Universally Unique Identifier) for identification purposes. Below is a depiction of the BLE hierarchy.

![Hierarchy BLE](/nodes/images/ble_profile.png "Hierarchy BLE")

Hierarchical Elements:

- Profile: A standardized suite of services for a specific application.
- Service: A bundle of related functionalities, like sensor data or battery levels.
- Characteristic: The data container within a service.
- Descriptor: Supplementary information about the characteristic.
- Properties: Interaction rules for the characteristic value, such as read, write, notify, etc.

Each service within a BLE profile comprises at least one characteristic, though it may also reference other services. A characteristic is intrinsically linked to a service and serves as the repository for actual data within the hierarchy, known as the characteristic value. In addition to the characteristic value, every characteristic includes a characteristic declaration, which offers metadata about the data it represents.

A notable feature of characteristics is the 'notify' property. This property enables a characteristic to inform the client of any changes to its value. When enabled, this notification ensures that clients are kept up-to-date with the most current information without the need to continuously poll the characteristic for updates.

The properties describe how the characteristic value can be interacted with. Basically, it contains the operations and procedures that can be used with the characteristic:

- Broadcast
- Read
- Write without response
- Write
- Notify
- Indicate
- Authenticated Signed Writes
- Extended Properties

Of these, we only use "read" and "write", as you can see in the code below used on server.

Each service, characteristic, and descriptor have a UUID (Universally Unique Identifier). A UUID is a unique 128-bit (16 bytes) number with this the uuid can identify a particular service provided by a Bluetooth device. We generated our UUIDs on [UUID Generator Site](https://www.uuidgenerator.net). Here's an example where we used it on code:

```cpp
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
```

The following code snippet outlines the server setup process in ESP32:

```cpp
void BleServer::setup() {
    String deviceName = "GeoCache_" + String(nodeId);
    BLEDevice::init(deviceName.c_str());
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    pServer->setCallbacks(new MyBLEServerCallbacks(this));
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    //pCharacteristic->setMTU(512); // Set your desired MTU size here - only for BT 5.0 and above
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
```
In the setup, we created a BLE device called "Geocache_" followed by node ID. After that we set the BLE device as a server. After that we create a service for the BLE server with the UUID defined above. And then we set the characteristic for that service. And finally we can start the service, and the advertising, so other BLE devices can scan and find this BLE device.

Firstly, to test the communication we used a application named "nRF Connect" and we confirm that the results were shown in the image below.

![nRF](/nodes/images/nRF.jpeg "nRF")

### RSSI Obtainment (Scans)

Received Signal Strength Indicator (RSSI) is a crucial component in Bluetooth Low Energy (BLE) applications for estimating the proximity of devices. In the context of BLE-based geocaching, RSSI is instrumental in guiding users to the location of a geocache, especially in environments where GPS signals may be unreliable. This signal strength measurement enables a user's device to determine how close it is to the geocache, with higher RSSI values indicating lower distance so more closer.

To translate the RSSI into a distance measurement, a commonly used formula takes into account the signal strength at a one-meter distance, known as Measured Power, and the environmental factor N, which accounts for the signal's decrease over distance.

The formula is as follows:

![RSSI Formula](/nodes/images/formulaRSSI.png "RSSI Formula")

In this formula, Measured Power is a factory-calibrated constant indicating the RSSI value at a one-meter distance from the beacon. RSSI is the current signal strength measured by the receiver. The N factor represents the signal propagation constant or path-loss exponent, which varies depending on environmental factors like walls and other obstacles that could attenuate the signal.

As we said in the explanation of the formula, RSSI is not without its challenges. The signal strength can be significantly affected by various environmental factors, including physical obstructions, interference from other electronic devices, and the materials the signal must pass through. Additionally, the RSSI value can fluctuate if either the user's device or the geocache beacon is moving, leading to potentially inconsistent distance readings. The orientation of the transmitting and receiving antennas also impacts the RSSI, as certain angles may weaken the perceived signal strength.

Despite these challenges, advancements in filtering and processing techniques have improved the reliability of RSSI-based distance estimations. Kalman filters, for example, have been employed to smooth out the signal variance, resulting in more stable RSSI readings. Moreover, the use of a Measured Power constant, which is a factory-calibrated value representing the expected RSSI at one meter from the beacon, enhances the accuracy of these estimations.

The integration of these enhancements with trilateration algorithms has led to better localization performance. However, it's important to note that there can still be a notable error margin. In testing scenarios within controlled environments, the average error was found to be approximately 13.7%, illustrating that while effective, RSSI-based localization should be implemented with an understanding of its limitations.

While RSSI is a beneficial tool for BLE-based geocaching, providing a low-cost and energy-efficient solution, it requires careful calibration and filtering to counteract environmental effects and signal fluctuations. These improvements are vital for developing a robust and reliable geocaching experience where traditional GPS may fall short.

### Service and Characteristics

### BLE Notify (Our method) and User's message to the server

BLE Notify is a feature of the BLE protocol that allows a BLE peripheral device to asynchronously notify a connected device, about changes in its characteristics, for example. 
We decided to implement a simplified method instead of using this feature.

Before we present our idea, we need to understand the format of the messages we are sending to the server when the user finds the cache. The packet format is shown below:

![BLE Message Format](/images/BLEmessage_format.png "BLE Message Format")

The two most important fields for our explanation are the packet and user ID's, since the header is only used to occupy an empty space to make sure the message has the correct format and the type corresponds to an opening request.

The reason of having a packet ID will be described on the next paragraph. The User ID is used to identify the person playing and making sure they have the right to access the cache.

Now that the message form is presented, we can go through our BLE notify replacing method.
When the user founds the cache and their information is sent to the server, by writing on it's characteristic, the server needs a way of knowing the data has been updated. Our way of doing this was having a packet ID field which will certainly be different for every message sent. The packet ID is incremented every time the user sends a message. When the server receives the information, it verifies if the value on the characteristic has changed or not, guarantying that if it has, the new information is processed.

The function below illustrates the server's procedure for verifying changes in data:

```cpp
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
```

We came into the conclusion that our method is not the best way to do this but we used it as a workaround in order to do it faster. Ideally, the client should use BLE notify to inform the data has been changed on that characteristic.

### Messages between server and app


