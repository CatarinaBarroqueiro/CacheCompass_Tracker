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

  BLEDevice::init("ESP32_1"); // Nome do dispositivo anunciador
  BLEServer *pServer = BLEDevice::createServer();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  BLEAdvertisementData advData;
  advData.setCompleteServices(BLEUUID(SERVICE_UUID));
  pAdvertising->setAdvertisementData(advData);
  pAdvertising->start();
}

void loop() {
  // Outras lógicas do seu código aqui
    delay(2000);
}
