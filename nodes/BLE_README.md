![image](https://github.com/CatarinaBarroqueiro/CacheCompass_Tracker/assets/72763845/e9e85ffa-a6f8-47e3-96c6-9a82e558fce5)# BLE Concepts

# BLE

## Why BLE?

In geocaching is very important for the people playing to be able to guide themselves with some mechanism helping through the path. Normally the technology chosen for this goal is GPS. However, in certain locations the GPS signal might be weak or inexistent, leading to more difficulties on finding the stashes by the players.
Bluetooth Low Energy (BLE) comes in clutch in this situation, since it can be used between two devices with it, meaning we can use it to create a communication between the cache and the user's phone. This way, when the user gets close enough of the BLE range, he can be guided to reach the cache, eliminating concernings about potential GPS malfunctions.

Another problem with the usual geocaching is that bad actors can access the geocache, removing the items from it and not placing anything for the next people to find.
With our idea, this can be solved. Only when the person is really close to the cache, and confirms that they have found it, the cache opens. By clicking the confirmation button, this person's information will be sent from the it's BLE device to the BLE server. This information will then be sent using LoRa in order to review it and classify the person as authorized, or not, to open the cache.
This way, people not using the app or that are non-authorized, can not take what's inside the cache. 



## Concepts

### BLE Server and Client

In a BLE ecosystem, there are two roles: the server and the client. At the start we created two projects, one for the client and other for the server using two ESP32s, leveraging their capability to function as both sides. A BLE server is capable of advertising its presence to potential clients and holds data that can be read or modified by the latter. Upon detecting a server, the client can form a connection and begin receiving or sending data, enabling point-to-point communication.

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
- Descriptor (Optional): Supplementary information about the characteristic.
- Properties: Interaction rules for the characteristic value, such as read, write, notify, etc.

Each service within a BLE profile comprises at least one characteristic, though it may also reference other services. A characteristic is intrinsically linked to a service and serves as the repository for actual data within the hierarchy, known as the characteristic value. In addition to the characteristic value, every characteristic includes a characteristic declaration, which offers metadata about the data it represents.

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

The following code outlines the server setup process in ESP32:

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
In the setup, we created a BLE device called "Geocache_" followed by node ID. This BLE device would act as a server. After creating the BLE server, we also created a service with the UUID defined as above. Then we set the characteristic for that service. Finally we started the service, and also advertising it, so other BLE devices can scan/find and access the service provided by this BLE device.

Firstly, to test the communication we used a application named "nRF Connect" and we confirm that the results were shown in the image below.

![nRF](/nodes/images/nRF.jpeg "nRF")

### RSSI

Received Signal Strength Indicator (RSSI) is a crucial component in Bluetooth Low Energy (BLE) applications for estimating the proximity of devices. In the context of BLE-based geocaching, RSSI is instrumental in guiding users to the location of a geocache, especially in environments where GPS signals may be unreliable. This signal strength measurement enables a user's device to determine how close it is to the geocache, with higher RSSI values indicating lower distance so more closer.

To translate the RSSI into a distance measurement, a commonly used formula takes into account the signal strength at a one-meter distance, known as Measured Power, and the environmental factor N, which accounts for the signal's decrease over distance.

The formula is as follows:

![RSSI Formula](/nodes/images/formulaRSSI.png "RSSI Formula")

In this formula, Measured Power is a factory-calibrated constant indicating the RSSI value at a one-meter distance from the beacon. RSSI is the current signal strength measured by the receiver. The N factor represents the signal propagation constant or path-loss exponent, which varies depending on environmental factors like walls and other obstacles that could attenuate the signal.

As we said in the explanation of the formula, RSSI is not without its challenges. The signal strength can be significantly affected by various environmental factors, including physical obstructions, interference from other electronic devices, and the materials the signal must pass through. Additionally, the RSSI value can fluctuate if either the user's device or the geocache beacon is moving, leading to potentially inconsistent distance readings. The orientation of the transmitting and receiving antennas also impacts the RSSI, as certain angles may weaken the perceived signal strength.

Despite these challenges, advancements in filtering and processing techniques have improved the reliability of RSSI-based distance estimations. Kalman filters, for example, have been employed to smooth out the signal variance, resulting in more stable RSSI readings. Moreover, the use of a Measured Power constant, which is a factory-calibrated value representing the expected RSSI at one meter from the beacon, enhances the accuracy of these estimations.

The integration of these enhancements with trilateration algorithms has led to better localization performance. However, it's important to note that there can still be a notable error margin. In testing scenarios within controlled environments, the average error was found to be approximately 13.7%, illustrating that while effective, RSSI-based localization should be implemented with an understanding of its limitations.

While RSSI is a beneficial tool for BLE-based geocaching, providing a low-cost and energy-efficient solution, it requires careful calibration and filtering to counteract environmental effects and signal fluctuations. These improvements are vital for developing a robust and reliable geocaching experience where traditional GPS may fall short.


### BLE Notify (Our method) and User's message to the server

BLE Notify is a feature of the BLE protocol that allows a BLE peripheral device to asynchronously notify a connected device, about changes in it's characteristics, for example. 
We decided to implement a simplified method instead of using this feature.

Before we present our idea, we need to understand the format of the messages we are sending to the server when the user finds the cache. The packet format is shown below:

![BLE Message Format](/nodes/images/BLEmessage_format.png "BLE Message Format")

The two most important fields for our explanation are the packet and user ID's, since the header is only used to occupy an empty space to make sure the message has the correct format and the type corresponds to an opening request.

The reason of having a packet ID will be described on the next paragraph. The User ID is used to identify the person playing and making sure they have the right to access the cache.

Now that the message form is presented, we can go through our BLE notify replacing method.
When the user founds the cache and their information is sent to the server, by writing on the server's characteristic, the server needs a way of knowing the data has been updated. Our way of doing this was having a packet ID field which will certainly be different for every message sent. The packet ID is incremented every time the user sends a message. When the server receives the information, it verifies if the value on the characteristic has changed or not, guarantying that if it has, the new information is processed.

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


# APP 

## Context

The primary purpose of our mobile application is to integrate the client's Bluetooth Low Energy (BLE) capabilities with their mobile device. This integration enables users to simulate real-world scenarios and determine their proximity to a geocache. When a user is close to a cache, the user confirms that he founds the cache and a message is sent to the server to acknowledge the discovery. This confirmation allows the user to proceed to the next cache search.

For this implementation, we selected Android Studio as our development environment. Android Studio provides the essential tools required to establish communication between the app and the server. It allows for the necessary permissions to connect to a Bluetooth server, scan for geocaches, and access location services for scanning purposes.


## Login Page

![Login Page](/App/images/loginPage.jpg "Login Page")

### Database access

The login functionality of the app involves accessing the database to retrieve user information. The process begins when the user clicks the login button. The app then fetches the user ID from the database, as demonstrated in the following code:
 
```java
loginbtn.setOnClickListener(new View.OnClickListener() {
    @Override
    public void onClick(View v) {
        String userEmail = username.getText().toString();
        userPassword = password.getText().toString();

        // Call the method to get user data
        getUserData(userEmail);
    }
});
private void getUserData(String email) {
    new RequestTask().execute("http://51.20.64.70:3000/user/email?email=" + email);
}

```
Upon retrieving the user ID, the app checks if the provided password exists and matches the user's password. If successful, user's ID is stored and passed to the next page as we can see in the following code.
```java
@Override
protected void onPostExecute(String result) {
    super.onPostExecute(result);
    if (result != null) {
        String storedPassword = parseJsonResponse(result);
        if (storedPassword != null && userPassword.equals(storedPassword)) {
            userId = parseUserId(result);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //Toast.makeText(MainActivity.this, "Login Successful", Toast.LENGTH_SHORT).show();
                    Toast.makeText(MainActivity.this, "Login Successful + " + storedPassword, Toast.LENGTH_SHORT).show();
                    Toast.makeText(MainActivity.this, "ID + " + userId, Toast.LENGTH_SHORT).show();

                    openMainRoom(userId);
                }
            });
        } else {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(MainActivity.this, "Login Failed", Toast.LENGTH_SHORT).show();
                }
            });
        }
    } 
}
public void openMainRoom(short userId) {
    Intent intent = new Intent(this, MainRoom.class);
    intent.putExtra("userId", userId);
    startActivity(intent);
}
```
## Main Page

The next image shown is the layout of the main game page.
<!-- TODO Change image-->
![Main Page](/App/images/mainPage.jpg "Main Page")

### RSSI with Scans and GATT Connection

To start the scanning in order to obtain the RSSI values and eventually connecting to the server for transmitting user's information, we implemented a broadcast receiver whithin the "OnCreate" function of the page. This receiver is designed to capture signals from other devices engaged in advertising, as illustrated in the code below: 

```java
private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        private int consecutiveLowDistanceCount = 0;

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            // ... Bluetooth Permission granting ...

            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                short rssi = intent.getShortExtra(BluetoothDevice.EXTRA_RSSI, Short.MAX_VALUE);

                String deviceName = device.getName();
                if (deviceName != null && isGeoCacheDevice(deviceName)) {
                    double distance = calculateDistance(rssi);

            // ...
```

To provide users with a more or less accurate estimation of their distance from the geocache, the distance is calculated using the RSSI value obtained during the scan, as outlined in the BLE RSSI chapter.

```java
private double calculateDistance(int rssi) {
    double RSSI_0 = -59.0; // Adjust this value based on your specific environment
    double N = 2.0;
    return (Math.pow(10, ((RSSI_0 - rssi) / (10.0 * N))));
}
```

We have defined a scan period of 1 second and introduced a new variable, "consecutiveLowDistanceCount", to ensure that the confirmation pop-up for discovering the geocache only appears when the user is in close proximity to the geocache for 2 or more seconds. Once the user confirms the geocache has been found, the user's device establishes a connection with the geocache using GATT. 

Upon successfully establishing the connection, service discovery is initiated to access the server characteristic for writing user's information:

```java
@Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (newState == BluetoothGatt.STATE_CONNECTED) {
                    onConnected();

                    if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
                        return;
                    }

                    gatt.discoverServices();
                }
            } else {
                showToastOnUIThread("Connection failed with status: " + status);
            }
        }
```


### Sending user's information to server

The user ID obtained from the login page is passed to the main game page to write on the characteristic. Upon locating a cache, the app uses the characteristic to send a message to the server indicating the discovery, allowing the user to proceed to the next cache.

The following code represents the entire process referred in the BLE Notify and User's message to the server chapter, of creating the message with the right format using user's information and writing it on the server's characteristic to check the user's right to open the geocache.

```java
@Override
public void onServicesDiscovered(BluetoothGatt gatt, int status) {
    if (status == BluetoothGatt.GATT_SUCCESS) {
        BluetoothGattService service = gatt.getService(SERVICE_UUID);
        if (service != null) {
            // Continue to discover characteristics within this service
            BluetoothGattCharacteristic characteristic = service.getCharacteristic(CHARACTERISTIC_UUID);
            if (characteristic != null) {
                test = characteristic.getUuid().toString();
                showToastOnUIThread(test);
                byte[] messageData = createMessageData(packetId);
                showToastOnUIThread("Message to be sent: " + Arrays.toString(messageData));
                sendBleMessage(characteristic, messageData);
            }
        } else {
            showToastOnUIThread("Service not discovered!");
        }
    }
}
private byte[] createMessageData(int packetId) {
    final short messageType = 0x02;
    ByteBuffer buffer = ByteBuffer.allocate(MESSAGE_HEADER_SIZE + MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE + MESSAGE_USER_ID_SIZE);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putShort(messageType); // Cast to short as Java doesn't have unsigned types
    buffer.putInt(packetId);
    buffer.putShort(userId);
    return buffer.array();
}
private void sendBleMessage(BluetoothGattCharacteristic characteristic, byte[] data) {
    Log.d("BLE", "Sending data: " + Arrays.toString(data));
    if (characteristic != null && data != null) {
        if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
            return;
        }
        characteristic.setValue(data);
        boolean success = gatt.writeCharacteristic(characteristic);
        if (success) {
            showToastOnUIThread("Message sent successfully");
        } else {
            showToastOnUIThread("Failed to send message");
        }
    } else {
        showToastOnUIThread("Characteristic or data is null");
    }
}
```

The app defines specific UUIDs for the service and characteristic, along with the sizes for various components of the message, ensuring consistency in communication.

The app pays special attention to the format of the messages sent via BLE as illustrated in the following code:

```java
private static final UUID SERVICE_UUID = UUID.fromString("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
private static final UUID CHARACTERISTIC_UUID = UUID.fromString("beb5483e-36e1-4688-b7f5-ea07361b26a8");

private static final int MESSAGE_HEADER_SIZE = 3;
private static final int MESSAGE_TYPE_SIZE = 2;
private static final int MESSAGE_PACKET_ID_SIZE = 4;
private static final int MESSAGE_USER_ID_SIZE = 2;
```

These specifications are crucial for the proper functioning of the app's BLE communication features.


### Disconnecting, restart Scans

After finding the geocache, user can click on the clear button, to empty the geocaches list, restart the scans and look for other geocaches. The code snippet below shows the disconnect done after this button click.

```java
private void disconnectFromServer() {
        if (gatt != null) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
                return;
            }
            gatt.disconnect();
            gatt.close(); // Optionally, close the GATT client completely
            gatt = null;
            isConnected = false;
        }
    }
```

The Bluetooth Adapter then starts the discovery back again. The following function shows all the process together.

```java
public void clearScannedList(View view) {
        deviceDetailsList.clear();
        arrayAdapter.notifyDataSetChanged();

        if (isConnected) {
            disconnectFromServer(); // Disconnect from the server
            packetId++; // Increment the packetId for the new scan

            // Reset states to allow new scan
            isConnected = false;
            shouldContinueScanning = true;
            alreadyPopup = false;
            
            // ... Permission granting ...
            
            if (mBluetoothAdapter != null && !mBluetoothAdapter.isDiscovering()) {
                
                // ... Permission granting ...

                mBluetoothAdapter.startDiscovery();
            }
        }
    }

```