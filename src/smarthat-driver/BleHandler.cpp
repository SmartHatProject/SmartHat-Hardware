#include "BleHandler.h"
#include "Message.h"
#include "DustSensor.h"



//UUIDS
#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define SOUND_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define DUST_CHARACTERISTIC_UUID  "dcba4321-8765-4321-8765-654321fedcba"
#define MAX_MTU_SIZE 512 

// moved CharacteristicCallbacks class definition to top 
// class was being used before its definition,
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

// ServerCallbacks methods
ServerCallbacks::ServerCallbacks(bool* connected) : deviceConnected(connected) {}

/*
 * Connection Callback
 * mtu negotiation
 * default too small
 * 
 * 
 */
void ServerCallbacks::onConnect(BLEServer* pServer) {
    *deviceConnected = true;
    Serial.println("\n=== BLE CLIENT CONNECTED ===");
    
    // request larger mtu size for efficient data transfer
    uint16_t mtu = pServer->getPeerMTU(pServer->getConnId());
    Serial.print("Default MTU: "); 
    Serial.println(mtu);
    
    // updatePeerMTU returns void  can't check if it succeeded
    pServer->updatePeerMTU(pServer->getConnId(), MAX_MTU_SIZE);
    Serial.print("Requested larger MTU: ");
    Serial.println(MAX_MTU_SIZE);
    
    Serial.println("Waiting for client to enable notifications...");
}

void ServerCallbacks::onDisconnect(BLEServer* pServer) {
    *deviceConnected = false;
    Serial.println("\n=== BLE CLIENT DISCONNECTED ===");
    // Restart advertising to allow new connections
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
    
    // Create server with error checking
    pServer = BLEDevice::createServer();
    if (pServer == nullptr) {
        Serial.println("ERROR: Failed to create BLE server");
        return;
    }
    
    // Set connection callbacks
    pServer->setCallbacks(new ServerCallbacks(&deviceConnected));
    
    // Create service with error checking
    pService = pServer->createService(BLEUUID(SERVICE_UUID));
    if (pService == nullptr) {
        Serial.println("ERROR: Failed to create BLE service");
        return;
    }

    // Sound Characteristic 
    pSoundCharacteristic = pService->createCharacteristic(
        BLEUUID(SOUND_CHARACTERISTIC_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    
    // Check if sound characteristic creation was successful
    if (pSoundCharacteristic == nullptr) {
        Serial.println("ERROR: Failed to create sound characteristic");
        return;
    }

    // Dust Characteristic
    pDustCharacteristic = pService->createCharacteristic(
        BLEUUID(DUST_CHARACTERISTIC_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    
    // Check if dust characteristic creation was successful
    if (pDustCharacteristic == nullptr) {
        Serial.println("ERROR: Failed to create dust characteristic");
        return;
    }

    
    Message initialSoundMessage = Message(Message::SOUND_SENSOR_DATA, 40.0f); // Default to 40 dB (quiet room)
    Message initialDustMessage = Message(Message::DUST_SENSOR_DATA, 10.0f);  // Default to 10 µg/m³ (clean air)
    
    String soundJson = initialSoundMessage.getJsonMessage();
    String dustJson = initialDustMessage.getJsonMessage();
    
    // Set initial values 
    if (soundJson.length() > 0) {
        pSoundCharacteristic->setValue(soundJson.c_str());
        Serial.println("Set initial sound characteristic value");
    } else {
        // Fallback if JSON creation fails
        const char* fallbackJson = "{\"messageType\":\"SOUND_SENSOR_DATA\",\"data\":40.0,\"timeStamp\":0}";
        pSoundCharacteristic->setValue(fallbackJson);
        Serial.println("Set fallback sound characteristic value");
    }
    
    if (dustJson.length() > 0) {
        pDustCharacteristic->setValue(dustJson.c_str());
        Serial.println("Set initial dust characteristic value");
    } else {
        // Fallback if JSON creation fails
        const char* fallbackJson = "{\"messageType\":\"DUST_SENSOR_DATA\",\"data\":10.0,\"timeStamp\":0}";
        pDustCharacteristic->setValue(fallbackJson);
        Serial.println("Set fallback dust characteristic value");
    }

   
    // sound sensor
    BLE2902* soundDescriptor = new BLE2902();
    if (soundDescriptor == nullptr) {
        Serial.println("ERROR: Failed to create sound descriptor");
        return;
    }
    
    soundDescriptor->setNotifications(true);
    bool soundDescriptorAdded = true;
    try {
        pSoundCharacteristic->addDescriptor(soundDescriptor);
    } catch (...) {
        soundDescriptorAdded = false;
        Serial.println("ERROR: Exception while adding sound descriptor");
    }
    
    if (!soundDescriptorAdded) {
        Serial.println("ERROR: Failed to add sound descriptor");
        delete soundDescriptor;
    }
    
    // dust sensor
    BLE2902* dustDescriptor = new BLE2902();
    if (dustDescriptor == nullptr) {
        Serial.println("ERROR: Failed to create dust descriptor");
        return;
    }
    
    dustDescriptor->setNotifications(true);
    bool dustDescriptorAdded = true;
    try {
        pDustCharacteristic->addDescriptor(dustDescriptor);
    } catch (...) {
        dustDescriptorAdded = false;
        Serial.println("ERROR: Exception while adding dust descriptor");
    }
    
    if (!dustDescriptorAdded) {
        Serial.println("ERROR: Failed to add dust descriptor");
        delete dustDescriptor; // Prevent memory leak
        return;
    }

    // Set callbacks 
    pSoundCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pDustCharacteristic->setCallbacks(new CharacteristicCallbacks());
    
    // Start the service 
    pService->start();
    Serial.println("BLE service started");

    /*
     * enhanced advertising
     * 
     * advertising parameters 
     *
     */
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    
   
    try {
      
        pAdvertising->setMinPreferred(0x12);   
    } catch (...) {
        Serial.println("WARNING: Could not set preferred connection parameters - library may not support it");
    }
    
    BLEDevice::startAdvertising();

    Serial.println("\n=== BLE Setup Complete ===");
    Serial.println("Device Name: SmartHat");
    Serial.println("Service UUID: " + String(SERVICE_UUID));
    Serial.println("Sound Characteristic UUID: " + String(SOUND_CHARACTERISTIC_UUID));
    Serial.println("Dust Characteristic UUID: " + String(DUST_CHARACTERISTIC_UUID));
    Serial.println("Waiting for connections...");
}


void BleHandler::updateSoundLevel(float soundLevel) {
    
    if (deviceConnected) {
       
        Message soundMessage = Message(Message::SOUND_SENSOR_DATA, soundLevel);
        String jsonMessage = soundMessage.getJsonMessage();

        Serial.println("\n=== Sound Update ===");
        Serial.print("Raw Value: "); 
        Serial.println(soundLevel, 2);
        Serial.print("JSON to send: "); 
        Serial.println(jsonMessage);
        
    
        if (jsonMessage.length() == 0) {
            Serial.println("ERROR: Empty JSON message for sound data");
            return;
        }
        
        // Set the value 
        pSoundCharacteristic->setValue(jsonMessage.c_str());
        Serial.println("Set sound characteristic value");
        
        // Send notification 
        pSoundCharacteristic->notify();
        Serial.println("Sound notification sent");
    }
}


void BleHandler::updateDustLevel(float dustLevel) {
    
    if (deviceConnected) {
        //create message from dust sensor and format in JSON
        Message dustMessage = Message(Message::DUST_SENSOR_DATA, dustLevel);
        String jsonMessage = dustMessage.getJsonMessage();

        Serial.println("\n=== Dust Update ===");
        Serial.print("Raw Value: "); 
        Serial.println(dustLevel, 2);
        Serial.print("JSON to send: ");
        Serial.println(jsonMessage);
        
        // Check if the JSON message is valid
        if (jsonMessage.length() == 0) {
            Serial.println("ERROR: Empty JSON message for dust data");
            return;
        }
        
        // Set the value 
        pDustCharacteristic->setValue(jsonMessage.c_str());
        Serial.println("Set dust characteristic value");
        
        // Send notification 
        pDustCharacteristic->notify();
        Serial.println("Dust notification sent");
    }
}


BLEServer* BleHandler::getServer() {
    return pServer;
}


BLEService* BleHandler::getService() {
    return pService;
}


BLECharacteristic* BleHandler::getSoundCharacteristic() {
    return pSoundCharacteristic;
}


BLECharacteristic* BleHandler::getDustCharacteristic() {
    return pDustCharacteristic;
}


bool BleHandler::isDeviceConnected() {
    return deviceConnected;
}

void BleHandler::setServer(BLEServer* server) {
    pServer = server;
}


void BleHandler::setService(BLEService* service) {
    pService = service;
}

void BleHandler::setSoundCharacteristic(BLECharacteristic* soundCharacteristic) {
    pSoundCharacteristic = soundCharacteristic;
}


void BleHandler::setDustCharacteristic(BLECharacteristic* dustCharacteristic) {
    pDustCharacteristic = dustCharacteristic;
}


