package com.example.cachecompass_tracker_app;

import android.Manifest;
import android.animation.ArgbEvaluator;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.UUID;

import kotlinx.coroutines.Delay;

public class MainRoom extends AppCompatActivity {

    private BluetoothAdapter mBluetoothAdapter;
    Intent btEnablingIntent;
    private ListView scannedListView;
    private ArrayList<String> deviceDetailsList = new ArrayList<>();
    private ArrayAdapter<String> arrayAdapter;
    private static final long SCAN_PERIOD = 1000; // New scan every second
    private Handler autoScanHandler;
    private boolean isConnected = false;
    private BluetoothGatt gatt;
    private boolean shouldContinueScanning = true;
    private Context appContext;
    private String test;

    private static final UUID SERVICE_UUID = UUID.fromString("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
    private static final UUID CHARACTERISTIC_UUID = UUID.fromString("beb5483e-36e1-4688-b7f5-ea07361b26a8");

    private static final int MESSAGE_HEADER_SIZE = 3;
    private static final int MESSAGE_TYPE_SIZE = 2;
    private static final int MESSAGE_PACKET_ID_SIZE = 4;
    private static final int MESSAGE_USER_ID_SIZE = 2;

    private int packetId = 4;
    private short userId;
    //private static final short USER_ID = 1; //  uint16_t
    private boolean alreadyPopup = false;
    private boolean flagPopup = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_room);

        scannedListView = findViewById(R.id.scannedListView);
        setupBluetooth();

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        userId = getIntent().getShortExtra("userId", (short) 0);
        //Toast.makeText(this, "ID ROOM + " + userId, Toast.LENGTH_SHORT).show();


        if (!mBluetoothAdapter.isEnabled()) {
            btEnablingIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivity(btEnablingIntent);
        }

        arrayAdapter = new ArrayAdapter<>(this, R.layout.list_item_layout, R.id.deviceDetailsTextView, deviceDetailsList);
        scannedListView.setAdapter(arrayAdapter);

        autoScanHandler = new Handler();
    }

    private void setupBluetooth() {
        // Permissions granted at runtime
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 1);
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.BLUETOOTH_SCAN}, 1);
        }

        registerReceiver(mReceiver, new IntentFilter(BluetoothDevice.ACTION_FOUND));
    }

    private void enableBluetooth() {
        if (mBluetoothAdapter == null) {
            // Device doesn't support Bluetooth
            Toast.makeText(getApplicationContext(), "Bluetooth not supported on this device", Toast.LENGTH_LONG).show();
        } else {
            if (!mBluetoothAdapter.isEnabled()) {
                // Enable Bluetooth
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 1);
                }
                startActivity(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE));
            } else {
                Toast.makeText(getApplicationContext(), "Bluetooth already enabled", Toast.LENGTH_LONG).show();
            }
        }
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        private int consecutiveLowDistanceCount = 0;

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 1);
            }

            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                short rssi = intent.getShortExtra(BluetoothDevice.EXTRA_RSSI, Short.MAX_VALUE);

                String deviceName = device.getName();
                if (deviceName != null && isGeoCacheDevice(deviceName)) {
                    double distance = calculateDistance(rssi);
                    // Format the distance with two decimal places
                    DecimalFormat decimalFormat = new DecimalFormat("#.##");
                    String formattedDistance = decimalFormat.format(distance);

                    // Check if the device is already in the list
                    boolean deviceFound = false;
                    for (int i = 0; i < deviceDetailsList.size(); i++) {
                        String existingDevice = deviceDetailsList.get(i);
                        if (existingDevice.contains(device.getName())) {
                            // Update RSSI value
                            deviceDetailsList.set(i, "Device Name: " + device.getName() + "\nRSSI: " + rssi + "\nDistance: " + formattedDistance + " meters");
                            deviceFound = true;

                            // Update signal icon color (corresponds to first geocache found)
                            if (i == 0) {
                                updateDistanceIndicatorColor(distance);
                            }

                            break; // NEED TO CHANGE?
                        }
                    }

                    // If the device is not in the list, add a new line
                    if (!deviceFound) {
                        deviceDetailsList.add("Device Name: " + device.getName() + "\nRSSI: " + rssi + "\nDistance: " + formattedDistance + " meters");
                    }

                    if (distance < 1.0 && !alreadyPopup) {
                        consecutiveLowDistanceCount++;

                        if (consecutiveLowDistanceCount >= 2) {
                            alreadyPopup = true;
                            stopScans();
                            showCacheFoundDialog(distance, device);
                        }
                    } else {
                        consecutiveLowDistanceCount = 0;
                    }
                }

                arrayAdapter.notifyDataSetChanged();
            }
        }

        private void showCacheFoundDialog(double distance, BluetoothDevice device) {
            AlertDialog.Builder builder = new AlertDialog.Builder(MainRoom.this);
            builder.setTitle("Cache Found");
            builder.setMessage("Did you find the cache?")
                    .setPositiveButton("Yes", (dialog, id) -> {
                        Toast.makeText(MainRoom.this, "Cache found!", Toast.LENGTH_SHORT).show();
                        connectToServer(device);

                        alreadyPopup = false; // Reset the flag for the next scan
                    })
                    .setNegativeButton("No", (dialog, id) -> {
                        Toast.makeText(MainRoom.this, "keep Looking!", Toast.LENGTH_SHORT).show();
                        flagPopup = false;
                        alreadyPopup = false;
                    });

            AlertDialog dialog = builder.create();
            dialog.show();
        }
    };

    private void stopScans() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {

            return;
        }
        if (mBluetoothAdapter != null && mBluetoothAdapter.isDiscovering()) {
            mBluetoothAdapter.cancelDiscovery();
            //showToast("Scans stopped");
        }
    }

    private void disconnectFromServer() {
        Toast.makeText(getApplicationContext(), "Desconectou", Toast.LENGTH_SHORT).show();
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

    private void connectToServer(BluetoothDevice device) {
        if (device == null) {
            Toast.makeText(getApplicationContext(), "DEVICE NULL!", Toast.LENGTH_SHORT).show();
            return;
        }

        //Toast.makeText(getApplicationContext(), "GETTING HERE!", Toast.LENGTH_SHORT).show();

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
            return;
        }
        //Toast.makeText(getApplicationContext(), "GETTING HERE2!", Toast.LENGTH_SHORT).show();

        gatt = device.connectGatt(this, false, gattCallback);
    }

    // Callback for BluetoothGatt events
    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {

            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (newState == BluetoothGatt.STATE_CONNECTED) {
                    //showToastOnUIThread("Connected to server!!!");
                    onConnected();
                    //showToastOnUIThread("AFTER ON CONNECTED!!!");
                    if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
                        return;
                    }
                    //showToastOnUIThread("AFTER PERMISSION!!!");
                    gatt.discoverServices();
                }
            } else {
                showToastOnUIThread("Connection failed with status: " + status);
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            //showToastOnUIThread("SERVICE Discovered!!!");
            if (status == BluetoothGatt.GATT_SUCCESS) {
                //showToastOnUIThread("SERVICE Gatt success!!!");

                BluetoothGattService service = gatt.getService(SERVICE_UUID);
                if (service != null) {
                    //showToastOnUIThread("SERVICE FOUND!!!");
                    // Continue to discover characteristics within this service
                    BluetoothGattCharacteristic characteristic = service.getCharacteristic(CHARACTERISTIC_UUID);
                    if (characteristic != null) {
                        test = characteristic.getUuid().toString();
                        //showToastOnUIThread("CHARACTERISTIC FOUND!!!");
                        showToastOnUIThread(test);
                        byte[] messageData = createMessageData(packetId);
                        showToastOnUIThread("Message to be sent: " + Arrays.toString(messageData));
                        sendBleMessage(characteristic, messageData);

                        //showToastOnUIThread("COMUNICOU?");
                    }
                } else {
                    showToastOnUIThread("Service not discovered!");
                }
            }
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
                    //showToastOnUIThread("Message sent successfully");
                } else {
                    showToastOnUIThread("Failed to send message");
                }
            } else {
                showToastOnUIThread("Characteristic or data is null");
            }
        }

    };

    private byte[] createMessageData(int packetId) {
        final short messageType = 0x02;
        ByteBuffer buffer = ByteBuffer.allocate(MESSAGE_HEADER_SIZE + MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE + MESSAGE_USER_ID_SIZE);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.putShort(messageType); // Cast to short as Java doesn't have unsigned types
        buffer.putInt(packetId);
        buffer.putShort(userId);
        return buffer.array();
    }


    private void onConnected() {
        isConnected = true;
        shouldContinueScanning = false;
    }


    private boolean isGeoCacheDevice(String deviceName) {
        // Define a regular expression pattern for GeoCache devices
        String pattern = "^GeoCache_\\d+$";

        // Check if the device name matches the pattern
        return deviceName.matches(pattern);
    }

    private double calculateDistance(int rssi) {
        double RSSI_0 = -59.0; // Adjust this value based on your specific environment
        double N = 2.0;
        return (Math.pow(10, ((RSSI_0 - rssi) / (10.0 * N))));
    }

    private int lastColor = Color.TRANSPARENT; // Initialize with a transparent color or any initial color

    private void updateDistanceIndicatorColor(double distance) {
        ImageView distanceIndicator = findViewById(R.id.distanceIndicator);

        // Define the color gradient based on distance
        int startColor, endColor;
        if (distance < 2.0) {
            startColor = lastColor == Color.TRANSPARENT ? ContextCompat.getColor(getApplicationContext(), R.color.green) : lastColor;
            endColor = ContextCompat.getColor(getApplicationContext(), R.color.green);
        } else if (distance < 6.0) {
            startColor = lastColor == Color.TRANSPARENT ? ContextCompat.getColor(getApplicationContext(), R.color.yellow) : lastColor;
            endColor = ContextCompat.getColor(getApplicationContext(), R.color.yellow);
        } else {
            startColor = lastColor == Color.TRANSPARENT ? ContextCompat.getColor(getApplicationContext(), R.color.red) : lastColor;
            endColor = ContextCompat.getColor(getApplicationContext(), R.color.red); // Keep the same color beyond 6.0 meters
        }

        // Animate the color transition
        ValueAnimator colorAnimator = ObjectAnimator.ofObject(distanceIndicator, "colorFilter", new ArgbEvaluator(), startColor, endColor);
        colorAnimator.setDuration(1000); // Adjust the duration as needed
        colorAnimator.addUpdateListener(animator -> {
            // Save the last color during the animation
            lastColor = (int) animator.getAnimatedValue();
        });
        colorAnimator.start();
    }

    public void clearScannedList(View view) {
        deviceDetailsList.clear();
        arrayAdapter.notifyDataSetChanged();
        Toast.makeText(this, "Scanned list cleared", Toast.LENGTH_SHORT).show();



        if (isConnected) {
            disconnectFromServer(); // Disconnect from the server
            packetId++; // Increment the packetId for the new scan

            // Reset states to allow new scan
            isConnected = false;
            shouldContinueScanning = true;
            alreadyPopup = false;
            flagPopup = false;

            // Start a new scan
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                return;
            }
            if (mBluetoothAdapter != null && !mBluetoothAdapter.isDiscovering()) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
                mBluetoothAdapter.startDiscovery();
                //Toast.makeText(getApplicationContext(), "SCANNING", Toast.LENGTH_LONG).show();
            }
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        autoScanHandler.postDelayed(autoScanRunnable, SCAN_PERIOD);
    }

    @Override
    protected void onPause() {
        super.onPause();
        autoScanHandler.removeCallbacks(autoScanRunnable);
    }

    private final Runnable autoScanRunnable = new Runnable() {
        @Override
        public void run() {
            if (mBluetoothAdapter != null && mBluetoothAdapter.isEnabled() && shouldContinueScanning) {
                startScan();
            }
            autoScanHandler.postDelayed(this, SCAN_PERIOD);
        }
    };

    private void startScan() {
        //Toast.makeText(getApplicationContext(), "conect?"+isConnected, Toast.LENGTH_LONG).show();

        // Check for location and Bluetooth permissions
        if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED ||
                ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED ||
                ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH_ADMIN) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN}, 1);
            return;
        }

        // Check if Bluetooth is enabled
        if (!mBluetoothAdapter.isEnabled()) {
            enableBluetooth();
            return;
        }

        // Check if GPS is enabled
        if (!checkEnableGPS()) {
            Toast.makeText(getApplicationContext(), "GPS Not Enabled, Please enable first", Toast.LENGTH_LONG).show();
            Intent gpsIntent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            startActivity(gpsIntent);
            return;
        }

        if(!isConnected) {
            // Start Discovery
            try {
                if (mBluetoothAdapter.isDiscovering()) {
                    mBluetoothAdapter.cancelDiscovery();
                }

                if (shouldContinueScanning) {
                    mBluetoothAdapter.startDiscovery();
                    //Toast.makeText(getApplicationContext(), "SCANNING", Toast.LENGTH_LONG).show();
                }
            } catch (Exception e) {
                Toast.makeText(getApplicationContext(), "Exception, Scan Again", Toast.LENGTH_LONG).show();
            }
        }
    }



    private boolean checkEnableGPS() {
        LocationManager locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        return locationManager != null && (locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER) || locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER));
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    private void showToastOnUIThread(String message) {
        runOnUiThread(() -> Toast.makeText(getApplicationContext(), message, Toast.LENGTH_LONG).show());
    }
}