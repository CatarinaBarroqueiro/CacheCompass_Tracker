package com.example.cachecompass_tracker_app;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.os.Parcelable;
import android.provider.Settings;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.Map;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;

import java.nio.charset.StandardCharsets;
import java.util.UUID;


public class MainRoom extends AppCompatActivity {

    private BluetoothAdapter mBluetoothAdapter;
    int requestEnableForBluetooth;
    Button buttonON, buttonOFF, scanButton;
    Intent btEnablingIntent;
    Map<String, String> BluetoothData;
    private int scanCount = 3;
    private BluetoothGatt mBluetoothGatt;
    ListView scannedListView;
    ArrayList<String> deviceDetailsList = new ArrayList<>();
    ArrayAdapter<String> arrayAdapter;
    private static final int PERMISSION_REQUEST_BLUETOOTH_CONNECT = 100;
    private Button buttonClear;

    private Handler autoScanHandler;
    private Runnable autoScanRunnable;
    private static final long SCAN_PERIOD = 5000; // 5 seconds
    private TextView tvCountdown;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_room);
        scannedListView = findViewById(R.id.scannedListView);
        tvCountdown = findViewById(R.id.tvCountdown);

        buttonClear = findViewById(R.id.buttonClear);
        buttonON = findViewById(R.id.buttonON);
        buttonOFF = findViewById(R.id.buttonOFF);
        scanButton = findViewById(R.id.buttonScan);

        btEnablingIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        requestEnableForBluetooth = 1;

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        bluetoothONMethod();
        bluetoothOFFMethod();

        // Permissions granted at runtime
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 1);
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.BLUETOOTH_SCAN}, 1);
        }

        registerReceiver(mReceiver, new IntentFilter(BluetoothDevice.ACTION_FOUND));
        registerReceiver(mReceiver, new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_STARTED));
        registerReceiver(mReceiver, new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED));

        bluetoothSCANMethod();

        arrayAdapter = new ArrayAdapter<>(getApplicationContext(), android.R.layout.simple_list_item_1, deviceDetailsList);
        scannedListView.setAdapter(arrayAdapter);
        autoScanHandler = new Handler();
        autoScanRunnable = new Runnable() {
            int countdown = 5; // 5 seconds countdown

            @Override
            public void run() {
                if (countdown == 0) {
                    if (mBluetoothAdapter != null && mBluetoothAdapter.isEnabled()) {
                        startScan();
                    }
                    countdown = 5; // Reset countdown
                } else {
                    countdown--;
                    runOnUiThread(() -> tvCountdown.setText(String.valueOf(countdown)));
                    autoScanHandler.postDelayed(this, 1000); // Update every second
                    return;
                }

                autoScanHandler.postDelayed(this, SCAN_PERIOD);
            }
        };
    }

    private void connectToDevice(BluetoothDevice device) {
        String SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
        String CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
            // Inicia a conexão com o dispositivo BLE
            mBluetoothGatt = device.connectGatt(this, false, new BluetoothGattCallback() {
                @Override
                public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                    super.onConnectionStateChange(gatt, status, newState);
                    if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_CONNECTED) {
                        // A conexão foi estabelecida
                        if (ActivityCompat.checkSelfPermission(MainRoom.this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                            // TODO: Consider calling
                            //    ActivityCompat#requestPermissions
                            // here to request the missing permissions, and then overriding
                            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                            //                                          int[] grantResults)
                            // to handle the case where the user grants the permission. See the documentation
                            // for ActivityCompat#requestPermissions for more details.
                            return;
                        }
                        runOnUiThread(() -> Toast.makeText(MainRoom.this, "Connected to " + device.getName(), Toast.LENGTH_SHORT).show());
                        // Inicia a descoberta de serviços no dispositivo conectado
                        gatt.discoverServices();
                    } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                        // A conexão foi desconectada
                        runOnUiThread(() -> Toast.makeText(MainRoom.this, "Disconnected from " + device.getName(), Toast.LENGTH_SHORT).show());
                        if (mBluetoothGatt != null) {
                            mBluetoothGatt.close();
                        }
                    }
                }

                @Override
                public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                    super.onServicesDiscovered(gatt, status);
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        // Os serviços foram descobertos
                        runOnUiThread(() -> Toast.makeText(MainRoom.this, "Services Discovered", Toast.LENGTH_SHORT).show());

                        // Aqui você pode interagir com os serviços e características do dispositivo
                        // Por exemplo, para ler uma característica:
                        BluetoothGattService service = gatt.getService(UUID.fromString(SERVICE_UUID));
                        if (service != null) {
                            BluetoothGattCharacteristic characteristic = service.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID));
                            if (characteristic != null) {
                                if (ActivityCompat.checkSelfPermission(MainRoom.this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                                    // TODO: Consider calling
                                    //    ActivityCompat#requestPermissions
                                    // here to request the missing permissions, and then overriding
                                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                    //                                          int[] grantResults)
                                    // to handle the case where the user grants the permission. See the documentation
                                    // for ActivityCompat#requestPermissions for more details.
                                    return;
                                }
                                gatt.readCharacteristic(characteristic);
                            }
                        }
                    }
                }

                @Override
                public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
                    super.onCharacteristicRead(gatt, characteristic, status);
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        // A característica foi lida
                        String value = new String(characteristic.getValue(), StandardCharsets.UTF_8);
                        runOnUiThread(() -> Toast.makeText(MainRoom.this, "Characteristic Read: " + value, Toast.LENGTH_SHORT).show());
                    }
                }

                // Implemente outros métodos de callback como onCharacteristicWrite, onCharacteristicChanged, etc., conforme necessário
            });
        } else {
            // Solicita as permissões necessárias ao usuário
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, PERMISSION_REQUEST_BLUETOOTH_CONNECT);
        }
    }

    private void bluetoothSCANMethod() {
        scanButton.setOnClickListener(view -> {
            if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, 1);
                return;
            }
            if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, 1);
                return;
            }

            if (!mBluetoothAdapter.isEnabled()) {
                if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 1);
                }
                startActivity(btEnablingIntent);
                Toast.makeText(getApplicationContext(), "Enabling the Bluetooth", Toast.LENGTH_LONG).show();
                return;
            }
            if (!CheckEnableGPS()) {
                Toast.makeText(getApplicationContext(), "GPS Not Enabled, Please enable first", Toast.LENGTH_LONG).show();
                Intent gpsIntent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                startActivity(gpsIntent);
                return;
            }
            try {
                if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.BLUETOOTH_SCAN}, 1);
                }
                if (mBluetoothAdapter.isDiscovering()) {
                    mBluetoothAdapter.cancelDiscovery();
                }
                scanCount = 1; //for ex change the scan count number to 3 then after pressing the scan button (once) it will scan three time 12*3=36 seconds
                mBluetoothAdapter.startDiscovery();
            } catch (Exception npe) {
                Toast.makeText(getApplicationContext(), "Exception, Scan Again", Toast.LENGTH_LONG).show();
            }
        });
    }

    // Broadcast Receiver
    public final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        String SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";

        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 1);
            }

            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                short rssi = intent.getShortExtra(BluetoothDevice.EXTRA_RSSI, Short.MAX_VALUE);
                Toast.makeText(getApplicationContext(), "Device Found " + device.getName() + " RSSI: " + rssi + "dBm ", Toast.LENGTH_SHORT).show();

                //input data to display
                deviceDetailsList.add("Device Name: " + device.getName() + "\nDevice Address: " + device.getAddress() + " RSSI: " + rssi);

                arrayAdapter.notifyDataSetChanged();
                device.fetchUuidsWithSdp();
            } else if (mBluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                Toast.makeText(getApplicationContext(), "Discovery Finished", Toast.LENGTH_SHORT).show();
                scanCount = scanCount - 1;
                if (scanCount > 0) {
                    mBluetoothAdapter.startDiscovery();
                }
            } else if (mBluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                Toast.makeText(getApplicationContext(), "Discovery Started", Toast.LENGTH_SHORT).show();
            } else if (BluetoothDevice.ACTION_UUID.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                Parcelable[] uuidExtra = intent.getParcelableArrayExtra(BluetoothDevice.EXTRA_UUID);

                if (uuidExtra != null) {
                    for (Parcelable parcelable : uuidExtra) {
                        if (((ParcelUuid) parcelable).getUuid().toString().equals(SERVICE_UUID)) {
                            // Se o UUID do serviço corresponder, adicione o dispositivo à lista
                            String deviceDetails = "Device Name: " + device.getName() +
                                    "\nDevice Address: " + device.getAddress();
                            deviceDetailsList.add(deviceDetails);
                            arrayAdapter.notifyDataSetChanged();
                            break;
                        }
                    }
                }
            }
        }
    };

    private boolean CheckEnableGPS() {
        LocationManager locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        boolean isEnabled;
        try {
            isEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER) || locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        } catch (Exception npe) {
            isEnabled = false;
        }
        return isEnabled;
    }

    @Override
    protected void onDestroy() {
        Toast.makeText(getApplicationContext(), "Toasting all nearby discovered ", Toast.LENGTH_SHORT).show();
        // Your loop for Toasting the discovered devices should be done here
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    // onActivityResult() method should also be included as per your requirement
    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == requestEnableForBluetooth) {
            if (resultCode == RESULT_OK) {
                Toast.makeText(getApplicationContext(), "Bluetooth is enabled", Toast.LENGTH_LONG).show();
            } else if (resultCode == RESULT_CANCELED) {
                Toast.makeText(getApplicationContext(), "Bluetooth enabling cancelled", Toast.LENGTH_LONG).show();
            }
        }
    }

    private void bluetoothONMethod() {
        buttonON.setOnClickListener(v -> {
            if (mBluetoothAdapter == null) {
                // Device doesn't support Bluetooth
                Toast.makeText(getApplicationContext(), "Bluetooth not supported on this device", Toast.LENGTH_LONG).show();
            } else {
                if (!mBluetoothAdapter.isEnabled()) {
                    //code for bluetooth enables
                    startActivity(btEnablingIntent);
                } else {
                    Toast.makeText(getApplicationContext(), "Bluetooth already enabled", Toast.LENGTH_LONG).show();
                }
            }
        });
    }

    private void bluetoothOFFMethod() {
        buttonOFF.setOnClickListener(view -> {
            if (mBluetoothAdapter.isEnabled()) {
                if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions((Activity) getApplicationContext(), new String[]{Manifest.permission.BLUETOOTH}, 1);
                }
                mBluetoothAdapter.disable();
                Toast.makeText(getApplicationContext(), "Bluetooth is Disabled", Toast.LENGTH_LONG).show();
            } else {
                Toast.makeText(getApplicationContext(), "Bluetooth is Already Disabled", Toast.LENGTH_LONG).show();
            }

        });
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

    private void startScan() {
        // Check for location permissions as they are required for Bluetooth scanning
        if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, 1);
            return;
        }
        if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, 1);
            return;
        }

        // Check if Bluetooth is enabled
        if (!mBluetoothAdapter.isEnabled()) {
            if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 1);
            }
            startActivity(btEnablingIntent);
            Toast.makeText(getApplicationContext(), "Enabling the Bluetooth", Toast.LENGTH_LONG).show();
            return;
        }

        // Check if GPS is enabled
        if (!CheckEnableGPS()) {
            Toast.makeText(getApplicationContext(), "GPS Not Enabled, Please enable first", Toast.LENGTH_LONG).show();
            Intent gpsIntent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            startActivity(gpsIntent);
            return;
        }

        // Start Discovery
        try {
            if (ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainRoom.this, new String[]{Manifest.permission.BLUETOOTH_SCAN}, 1);
            }
            if (mBluetoothAdapter.isDiscovering()) {
                mBluetoothAdapter.cancelDiscovery();
            }
            scanCount = 1; // Adjust this value if needed
            mBluetoothAdapter.startDiscovery();
        } catch (Exception e) {
            Toast.makeText(getApplicationContext(), "Exception, Scan Again", Toast.LENGTH_LONG).show();
        }
    }

}