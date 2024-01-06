# APP 

## Context

The primary purpose of our mobile application is to integrate the client's Bluetooth Low Energy (BLE) capabilities with their mobile device. This integration enables users to simulate real-world scenarios and determine their proximity to a geocache. When a user is close to a cache, the user confirms that he founds the cache and a message is sent to the server to acknowledge the discovery. This confirmation allows the user to proceed to the next cache search.

For this implementation, we selected Android Studio as our development environment. Android Studio provides the essential tools required to establish communication between the app and the server. It allows for the necessary permissions to connect to a Bluetooth server, scan for geocaches, and access location services for scanning purposes.


## Login Page

![Login Page](/App/images/loginPage.png "Login Page")

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
![Main Page](/App/images/main_page.png "Main Page")

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