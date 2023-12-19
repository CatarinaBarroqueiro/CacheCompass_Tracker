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

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.nio.ByteBuffer;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.UUID;

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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_room);

        scannedListView = findViewById(R.id.scannedListView);
        setupBluetooth();

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

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

                    // Connect to the server only if the distance is less than 1 and not already connected
                    if (distance < 1.0 && !isConnected) {
                        connectToServer(device);
                        stopScans(); // Stop further scans once connected
                    }
                }

                arrayAdapter.notifyDataSetChanged();
            }
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
            if (status == BluetoothGatt.GATT_SUCCESS) {
                BluetoothGattService service = gatt.getService(SERVICE_UUID);
                if (service != null) {
                    //showToastOnUIThread("SERVICE FOUND!!!");
                    // Continue to discover characteristics within this service
                    BluetoothGattCharacteristic characteristic = service.getCharacteristic(CHARACTERISTIC_UUID);
                    if (characteristic != null) {
                        test = characteristic.getUuid().toString();
                        //showToastOnUIThread("CHARACTERISTIC FOUND!!!");
                        showToastOnUIThread(test);

                        //byte[] messageData = createYourMessageData();
                        // TODO
                        //sendBleMessage(characteristic, messageData);
                    }
                }
            }
        }

        // Helper method to show Toast on the UI thread
        private void showToastOnUIThread(String message) {
            runOnUiThread(() -> Toast.makeText(getApplicationContext(), message, Toast.LENGTH_LONG).show());
        }

        private void sendBleMessage(BluetoothGattCharacteristic characteristic, byte[] data) {
            if (characteristic != null && data != null) {
                characteristic.setValue(data);
                if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
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

    };

    /*private byte[] createYourMessageData() {
        int packetId = generatePacketId(); // Implement your own logic to generate packet ID
        int userId = getUserId(); // Implement your own logic to get the user ID

        ByteBuffer buffer = ByteBuffer.allocate(MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE + MESSAGE_USER_ID_SIZE);
        buffer.put((byte) OPENING_REQUEST.ordinal()); // Assuming OPENING_REQUEST is an enum
        buffer.putInt(packetId);
        buffer.putShort((short) userId);

        return buffer.array();
    }

    // Example implementations for generatePacketId and getUserId (replace with your logic)
    private int generatePacketId() {
        return 123; // Replace with your actual logic
    }

    private int getUserId() {
        return 456; // Replace with your actual logic
    }*/


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
        return Math.pow(10, ((RSSI_0 - rssi) / (10.0 * N)));
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
        deviceDetailsList.clear(); // Limpa a lista de dispositivos
        arrayAdapter.notifyDataSetChanged(); // Notifica o adaptador sobre a mudança
        Toast.makeText(this, "Scanned list cleared", Toast.LENGTH_SHORT).show();
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
                    Toast.makeText(getApplicationContext(), "SCANNING", Toast.LENGTH_LONG).show();
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
}
