# APP 

## Context

The primary purpose of our mobile application is to integrate the client's Bluetooth Low Energy (BLE) capabilities with their mobile device. This integration enables users to simulate real-world scenarios and determine their proximity to a geocache. When a user is close to a cache, the user confirms that he founds the cache and send a message to the server to acknowledge the discovery. This confirmation allows the user to proceed to the next cache search.

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
Upon retrieving the user ID, the app checks if the provided password exists and matches the user's password. If successful, this variable is stored and passed to the next page as we can see in the following code.
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

The user ID obtained from the login page is passed to the main game page to write a characteristic. Upon locating a cache, the user use the characteristic to sends a message to the server to indicate their discovery, allowing them to proceed to the next cache.

The next image shown is the layout of the main game page.

![Login Page](/App/images/loginPage.jpg "Login Page")

The following code represents the entire process explained in the ble section, i.e. if it has found a geocache service it opens it, then does the same for the feature and finally creates the message format to be able to send the message to the devices with the found feature.

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

### RSSI
...

```java
private double calculateDistance(int rssi) {
    double RSSI_0 = -59.0; // Adjust this value based on your specific environment
    double N = 2.0;
    return (Math.pow(10, ((RSSI_0 - rssi) / (10.0 * N))));
}
```

### Connect and scan
...

### disconnect, new scan
...
