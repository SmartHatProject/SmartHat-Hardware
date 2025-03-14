#include "BleHandler.h"
#include "Message.h"

//UUIDS
#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define SOUND_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define DUST_CHARACTERISTIC_UUID  "dcba4321-8765-4321-8765-654321fedcba"


// ServerCallbacks methods
ServerCallbacks::ServerCallbacks(bool* connected) : deviceConnected(connected) {}

void ServerCallbacks::onConnect(BLEServer* pServer) {
    *deviceConnected = true;
    Serial.println("\n=== BLE CLIENT CONNECTED ===");
    Serial.println("Waiting for client to enable notifications...");
}

void ServerCallbacks::onDisconnect(BLEServer* pServer) {
    *deviceConnected = false;
    Serial.println("\n=== BLE CLIENT DISCONNECTED ===");
    BLEDevice::startAdvertising();
    Serial.println("Started advertising again");
}

// initialize 
BleHandler::BleHandler() {
    pServer = nullptr;
    pService = nullptr;
    pSoundCharacteristic = nullptr;
    pDustCharacteristic = nullptr;
    deviceConnected = false;
}

void BleHandler::setUpBle() {
    Serial.println("\n=== Starting BLE Setup ===");

    // Initialize the BLE device
    BLEDevice::init("SmartHat");
    pServer = BLEDevice::createServer();
    
    //connection callbacks
    pServer->setCallbacks(new ServerCallbacks(&deviceConnected));
    
    pService = pServer->createService(BLEUUID(SERVICE_UUID));

    // Sound Characteristic 
    pSoundCharacteristic = pService->createCharacteristic(
        BLEUUID(SOUND_CHARACTERISTIC_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );

    // Dust Characteristic (Read Only)
    pDustCharacteristic = pService->createCharacteristic(
        BLEUUID(DUST_CHARACTERISTIC_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );

    // Set initial value with proper JSON format this has to use this format previous one was wrong
    std::string initialValue = "{\"messageType\":\"SOUND_SENSOR_DATA\",\"data\":1.0,\"timeStamp\":0}";
    
    pSoundCharacteristic->setValue(initialValue);
    pDustCharacteristic->setValue("{\"messageType\":\"DUST_SENSOR_DATA\",\"data\":1.0,\"timeStamp\":0}");

    // Descriptors
    pSoundCharacteristic->addDescriptor(new BLE2902());
    pDustCharacteristic->addDescriptor(new BLE2902());

    // callbacks for debugging
    pSoundCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pDustCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pService->start();

   
    // Set up advertising with the service UUID
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    Serial.println("\n=== BLE Setup Complete ===");
    Serial.println("Service UUID: " + String(SERVICE_UUID));
    Serial.println("Sound Characteristic UUID: " + String(SOUND_CHARACTERISTIC_UUID));
    Serial.println("Dust Characteristic UUID: " + String(DUST_CHARACTERISTIC_UUID));
    Serial.println("Waiting for connections...");
}

// Method to update sound level (using a float value)
void BleHandler::updateSoundLevel(float soundLevel) {
    // Only send updates if a device is connected
    if (deviceConnected) {
        //create message from sound sensor and format in JSON
        Message soundMessage = Message("SOUND_SENSOR_DATA", soundLevel);
        std::string jsonMessage = soundMessage.getJsonMessage();

        //set the value of sound characteristic to JSON string so that android app can process
        pSoundCharacteristic->setValue(jsonMessage.c_str());
        pSoundCharacteristic->notify();

        //print for debug
        Serial.println("\n=== Sound Update ===");
        Serial.println("Value: " + String(soundLevel, 2));
        Serial.println("JSON: " + jsonMessage);
    }
}

// Method to update dust level (using a float value)
void BleHandler::updateDustLevel(float dustLevel) {
    // Only send updates if a device is connected
    if (deviceConnected) {
        //create message from dust sensor and format in JSON
        Message dustMessage = Message("DUST_SENSOR_DATA", dustLevel);
        std::string jsonMessage = dustMessage.getJsonMessage();

        //set the vakue of the dust characteristic to the JSON string so that the android app can process it
        pDustCharacteristic->setValue(jsonMessage.c_str());
        pDustCharacteristic->notify();

        //print for debugging purposes
        Serial.println("\n=== Dust Update ===");
        Serial.println("Value: " + String(dustLevel, 2));
        Serial.println("JSON: " + jsonMessage);
    }
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

// Get connection status
bool BleHandler::isDeviceConnected() {
    return deviceConnected;
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

// Characteristic callback implementation
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic* pCharacteristic) {
        String uuid = pCharacteristic->getUUID().toString().c_str();
        Serial.println("Characteristic read: " + uuid);
    }
    
    void onWrite(BLECharacteristic* pCharacteristic) {
        String uuid = pCharacteristic->getUUID().toString().c_str();
        Serial.println("Characteristic write: " + uuid);
        Serial.println("Value: " + String(pCharacteristic->getValue().c_str()));
    }
    
    void onNotify(BLECharacteristic* pCharacteristic) {
        String uuid = pCharacteristic->getUUID().toString().c_str();
        Serial.println("Notification sent: " + uuid);
    }
};


