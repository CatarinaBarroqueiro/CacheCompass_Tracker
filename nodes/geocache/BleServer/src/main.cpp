#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Exemplo de UUID de serviço
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Exemplo de UUID de característica

BLEServer *pServer = NULL;

void setup() {
  Serial.begin(115200);

  BLEDevice::init("ESP32_1"); // Name of the announcer
  BLEServer *pServer = BLEDevice::createServer();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  BLEAdvertisementData advData;
  advData.setCompleteServices(BLEUUID(SERVICE_UUID));
  pAdvertising->setAdvertisementData(advData);
  pAdvertising->start();
}

void loop() {
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();
    // pAdvertising->start();
    // delay(2000); // Advertise for 2 seconds (adjust as needed)
    // pAdvertising->stop();
    delay(2000); // Wait for 2 seconds before the next advertising cycle
}
