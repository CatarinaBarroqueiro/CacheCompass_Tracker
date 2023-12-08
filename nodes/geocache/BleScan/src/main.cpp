#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // UUID do serviço anunciado
#define RSSI_0 -59                                          // Valor de RSSI a uma distância conhecida
#define N 2.0                                               // Expoente de Atenuação do Caminho
// N é o Expoente de Atenuação do Caminho (Path Loss Exponent), 
// variando de 2 a 4 dependendo do ambiente (geralmente é próximo
// a 2 para áreas abertas e até 4 para áreas urbanas densas).

BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
public:
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.haveRSSI() && advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
            Serial.print("Dispositivo encontrado: ");
            Serial.println(advertisedDevice.toString().c_str());
            calculateAndPrintDistance(advertisedDevice);
        }
    }

private:
    void calculateAndPrintDistance(BLEAdvertisedDevice& device) {
        int rssi = device.getRSSI();
        Serial.print("RSSI: ");
        Serial.print(rssi);
        Serial.println(" dBm");

        float distance = pow(10, ((RSSI_0 - rssi) / (10.0 * N)));
        Serial.print("Distância estimada: ");
        Serial.println(distance);
    }
};
void setup() {
    Serial.begin(115200);

    BLEDevice::init("ESP32_2"); // Scanner BLE
    pBLEScan = BLEDevice::getScan(); // Initialize BLE scan object
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
}

void loop() {
    // Scan devices for 1 sec and delay for 1 sec
    BLEScanResults foundDevices = pBLEScan->start(1);
    pBLEScan->clearResults();  
    delay(1000); // Delay: save energy and wait for person to move
}