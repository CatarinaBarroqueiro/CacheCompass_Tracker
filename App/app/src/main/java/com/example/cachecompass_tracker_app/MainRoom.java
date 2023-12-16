package com.example.cachecompass_tracker_app;

import android.Manifest;
import android.animation.ArgbEvaluator;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
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
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.text.DecimalFormat;
import java.util.ArrayList;

public class MainRoom extends AppCompatActivity {

    private BluetoothAdapter mBluetoothAdapter;
    Intent btEnablingIntent;
    private ListView scannedListView;
    private ArrayList<String> deviceDetailsList = new ArrayList<>();
    private ArrayAdapter<String> arrayAdapter;
    private static final long SCAN_PERIOD = 1000; // New scan every second
    private Handler autoScanHandler;

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

                            break;
                        }
                    }

                    // If the device is not in the list, add a new line
                    if (!deviceFound) {
                        deviceDetailsList.add("Device Name: " + device.getName() + "\nRSSI: " + rssi + "\nDistance: " + formattedDistance + " meters");
                    }
                }

                arrayAdapter.notifyDataSetChanged();
            }
        }
    };

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
            if (mBluetoothAdapter != null && mBluetoothAdapter.isEnabled()) {
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

        // Start Discovery
        try {
            if (mBluetoothAdapter.isDiscovering()) {
                mBluetoothAdapter.cancelDiscovery();
            }
            Toast.makeText(getApplicationContext(), "SCANNING", Toast.LENGTH_LONG).show();
            mBluetoothAdapter.startDiscovery();
        } catch (Exception e) {
            Toast.makeText(getApplicationContext(), "Exception, Scan Again", Toast.LENGTH_LONG).show();
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
