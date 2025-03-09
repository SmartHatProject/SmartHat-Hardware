#include "BLEHandler.h"

#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define SOUND_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define DUST_CHARACTERISTIC_UUID  "dcba4321-8765-4321-8765-654321fedcba"

// Constructor: Initialize pointers
BleHandler::BleHandler() {
    pServer = nullptr;
    pService = nullptr;
    pSoundCharacteristic = nullptr;
    pDustCharacteristic = nullptr;
}

void BleHandler::setUpBle() {
    Serial.println("\nStarting BLE work!");

    // Initialize the BLE device
    BLEDevice::init("SmartHat");
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);

    // Sound Characteristic 
    pSoundCharacteristic = pService->createCharacteristic(
        SOUND_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ| BLECharacteristic::PROPERTY_NOTIFY
    );

    // Set initial sound level (example: 50.0f)
    float initialSoundLevel = 0.0f;
    pSoundCharacteristic->setValue(reinterpret_cast<uint8_t*>(&initialSoundLevel), sizeof(initialSoundLevel));

    // Dust Characteristic (Read Only)
    pDustCharacteristic = pService->createCharacteristic(
        DUST_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ| BLECharacteristic::PROPERTY_NOTIFY
    );

    // Set initial dust level = 0.0
    float initialDustLevel = 0.0f;
    pDustCharacteristic->setValue(reinterpret_cast<uint8_t*>(&initialDustLevel), sizeof(initialDustLevel));

    // Optional: Add descriptor for client to receive notifications (for Sound level)
    pSoundCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    BLEDevice::startAdvertising();

    Serial.println("\nBLE Service with multiple characteristics started!");
}

// Method to update sound level (using a float value)
void BleHandler::updateSoundLevel(float soundLevel) {
    pSoundCharacteristic->setValue(reinterpret_cast<uint8_t*>(&soundLevel), sizeof(soundLevel));
    Serial.println("\nSound Level updated: " + String(soundLevel));
}

// Method to update dust level (using a float value)
void BleHandler::updateDustLevel(float dustLevel) {
    pDustCharacteristic->setValue(reinterpret_cast<uint8_t*>(&dustLevel), sizeof(dustLevel));
    Serial.println("\nDust Level updated: " + String(dustLevel));
}

// Getter for the BLEServer
BLEServer* BleHandler::getServer() {
    return pServer;
}

// Getter for the BLEService
BLEService* BleHandler::getService() {
    return pService;
}

// Getter for the Sound Characteristic
BLECharacteristic* BleHandler::getSoundCharacteristic() {
    return pSoundCharacteristic;
}

// Getter for the Dust Characteristic
BLECharacteristic* BleHandler::getDustCharacteristic() {
    return pDustCharacteristic;
}

// Setter for the BLEServer
void BleHandler::setServer(BLEServer* server) {
    pServer = server;
}

// Setter for the BLEService
void BleHandler::setService(BLEService* service) {
    pService = service;
}

// Setter for the Sound Characteristic
void BleHandler::setSoundCharacteristic(BLECharacteristic* soundCharacteristic) {
    pSoundCharacteristic = soundCharacteristic;
}

// Setter for the Dust Characteristic
void BleHandler::setDustCharacteristic(BLECharacteristic* dustCharacteristic) {
    pDustCharacteristic = dustCharacteristic;
}


