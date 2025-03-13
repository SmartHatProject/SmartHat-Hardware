#include "BleHandler.h"
#include "Message.h"


#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define SOUND_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define DUST_CHARACTERISTIC_UUID  "dcba4321-8765-4321-8765-654321fedcba"


/*
* will add a call back class later to see 
* if device(our app) connected/disconnected
* needed
*/






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

    // Dust Characteristic (Read Only)
    pDustCharacteristic = pService->createCharacteristic(
        DUST_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ| BLECharacteristic::PROPERTY_NOTIFY
    );

    //set initial value of sound and dust characteristics to empty JSON
    std::string initialSoundCharacteristicValue = "{}"; // Replace this with th code in the comment below
    /* TRY:
    * std::string initialSoundCharacteristicValue = "{\"messageType\":\"SOUND_SENSOR_DATA\",\"data\":0.0,\"timeStamp\":" + String(millis()).c_str() + "}";
    *SAME for dust
    */
    pSoundCharacteristic->setValue(initialSoundCharacteristicValue.c_str());
    
    std::string initialDustCharacteristicValue = "{}";
    pDustCharacteristic->setValue(initialDustCharacteristicValue.c_str());

    // Optional: Add descriptor for client to receive notifications (for Sound level)
    pSoundCharacteristic->addDescriptor(new BLE2902());
    pDustCharacteristic->addDescriptor(new BLE2902());
    /*
    *when notify
    *also need BLE2902 characteristic
    *to push updates to app
    *init with empty json setValue("{}")------------ we call the notify function in the update function so no need for that here - Anthony. 
    */
    


    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    BLEDevice::startAdvertising();

    Serial.println("\nBLE Service with multiple characteristics started!");
}

// Method to update sound level (using a float value)
void BleHandler::updateSoundLevel(float soundLevel) {

    //create message from sound sensor and format in JSON
    Message soundMessage = Message(Message::SOUND_DATA_MESSAGE, soundLevel);
    std::string jsonMessage = soundMessage.getJsonMessage();

    //set the value of sound characteristic to JSON string so that android app can process
    pSoundCharacteristic->setValue(jsonMessage.c_str());
    pSoundCharacteristic->notify();

    //print for debug
    Serial.println("value of sound sensor characteristic sent to app: ");
    Serial.println(jsonMessage.c_str());  
}

// Method to update dust level (using a float value)
void BleHandler::updateDustLevel(float dustLevel) {
    
    //create message from dust sensor and format in JSON
    Message dustMessage = Message(Message::DUST_DATA_MESSAGE, dustLevel);
    std:: string jsonMessage=dustMessage.getJsonMessage();

    //set the vakue of the dust characteristic to the JSON string so that the android app can process it
    pDustCharacteristic->setValue(jsonMessage.c_str());
    pDustCharacteristic->notify();

    //print for debugging purposes
    Serial.println("value of dust sensor characteristic sent to app: ");
    Serial.println(jsonMessage.c_str());
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


