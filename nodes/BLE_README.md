![image](https://github.com/CatarinaBarroqueiro/CacheCompass_Tracker/assets/72763845/e9e85ffa-a6f8-47e3-96c6-9a82e558fce5)# BLE Concepts

## Why BLE?

In geocaching is very important for the people to be able to guide themselves with some mechanism helping through the path. Normally the technology chosen for this goal is GPS. However, in certain locations the GPS signal might be weak or inexistent, leading to more difficulties on finding the stashes by the players.
Bluetooth Low Energy (BLE) comes in clutch in this situation, since it can be used between two devices with it, meaning we can use it to create a communication between the cache and the user's phone. This way, when the user gets close enough of the BLE range, he can be guided to the cache, without having to worry about GPS functioning.

Another problem with the usual geocaching is that bad actors can access the geocache, removing the items from it and not placing anything for the next people to find.
With our idea, this can be solved. Only when the person is really close to the cache, and confirms that he has found it, the cache opens. By clicking the confirmation, this person's information will be sent from the it's BLE device to the BLE server, that will after communicate by LoRa with the main website, in order to review the information sent and classify the person as authorized, or not, to open the cache.
This way, people not using the app or that are non-authorized, can not take what's inside the cache. 



## Concepts

### RSSI Obtainment (Scans)

Received Signal Strength Indicator (RSSI) is a crucial component in Bluetooth Low Energy (BLE) applications for estimating the proximity of devices. In the context of BLE-based geocaching, RSSI is instrumental in guiding users to the location of a geocache, especially in environments where GPS signals may be unreliable. This signal strength measurement enables a user's device to determine how close it is to the geocache, with higher RSSI values indicating closer proximity.

To translate the RSSI into a distance measurement, a commonly used formula takes into account the signal strength at a one-meter distance, known as Measured Power, and the environmental factor N, which accounts for the signal's decrease over distance.

The formula is as follows:

![RSSI Formula](/nodes/images/formulaRSSI.png "RSSI Formula")

In this formula, Measured Power is a factory-calibrated constant indicating the RSSI value at a one-meter distance from the beacon. RSSI is the current signal strength measured by the receiver. The N factor represents the signal propagation constant or path-loss exponent, which varies depending on environmental factors like walls and other obstacles that could attenuate the signal.

As we said in the explanation of the formula, RSSI is not without its challenges. The signal strength can be significantly affected by various environmental factors, including physical obstructions, interference from other electronic devices, and the materials the signal must pass through. Additionally, the RSSI value can fluctuate if either the user's device or the geocache beacon is moving, leading to potentially inconsistent distance readings. The orientation of the transmitting and receiving antennas also impacts the RSSI, as certain angles may weaken the perceived signal strength.

Despite these challenges, advancements in filtering and processing techniques have improved the reliability of RSSI-based distance estimations. Kalman filters, for example, have been employed to smooth out the signal variance, resulting in more stable RSSI readings. Moreover, the use of a Measured Power constant, which is a factory-calibrated value representing the expected RSSI at one meter from the beacon, enhances the accuracy of these estimations.

The integration of these enhancements with trilateration algorithms has led to better localization performance. However, it's important to note that there can still be a notable error margin. In testing scenarios within controlled environments, the average error was found to be approximately 13.7%, illustrating that while effective, RSSI-based localization should be implemented with an understanding of its limitations.

While RSSI is a beneficial tool for BLE-based geocaching, providing a low-cost and energy-efficient solution for indoor localization, it requires careful calibration and filtering to counteract environmental effects and signal fluctuations. These improvements are vital for developing a robust and reliable geocaching experience where traditional GPS may fall short.

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

### GATT


