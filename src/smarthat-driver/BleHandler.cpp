#include "BleHandler.h"
#include "Message.h"


#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define SOUND_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define DUST_CHARACTERISTIC_UUID  "dcba4321-8765-4321-8765-654321fedcba"
#define GAS_CHARACTERISTIC_UUID  "b6fc48af-6b61-4f96-afdf-a359a8b2b1b1"


#define MAX_MTU_SIZE 512 

// moved CharacteristicCallbacks class definition to top 
// class was being used before its definition,
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic* pCharacteristic) {
        String uuid = pCharacteristic->getUUID().toString().c_str();
        // Serial.println("Characteristic read: " + uuid);
    }
    
    void onWrite(BLECharacteristic* pCharacteristic) {
        String uuid = pCharacteristic->getUUID().toString().c_str();
        // Serial.println("Characteristic write: " + uuid);
        // Serial.println("Value: " + String(pCharacteristic->getValue().c_str()));
    }
    
    void onNotify(BLECharacteristic* pCharacteristic) {
        String uuid = pCharacteristic->getUUID().toString().c_str();
        // Serial.println("Notification sent: " + uuid);
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
    // Serial.print("Default MTU: "); 
    // Serial.println(mtu);
    
    // updatePeerMTU returns void  can't check if it succeeded
    pServer->updatePeerMTU(pServer->getConnId(), MAX_MTU_SIZE);
    // Serial.print("Requested larger MTU: ");
    // Serial.println(MAX_MTU_SIZE);
    
    // Serial.println("Waiting for client to enable notifications...");
}

void ServerCallbacks::onDisconnect(BLEServer* pServer) {
    *deviceConnected = false;
    Serial.println("\n=== BLE CLIENT DISCONNECTED ===");
    // Restart advertising to allow new connections
    BLEDevice::startAdvertising();
    Serial.println("Started advertising again");
}




// Constructor: Initialize pointers
BleHandler::BleHandler() {
    pServer = nullptr;
    pService = nullptr;
    pSoundCharacteristic = nullptr;
    pDustCharacteristic = nullptr;
    pGasCharacteristic = nullptr;
}

void BleHandler::setUpBle() {
    // Serial.println("\nStarting BLE work!");

    // Initialize the BLE device
    BLEDevice::init("SmartHat");

    pServer = BLEDevice::createServer();
    if (pServer == nullptr) {
        // Serial.println("ERROR: Failed to create BLE server");
        return;
    }

    pServer->setCallbacks(new ServerCallbacks(&deviceConnected));

    pService = pServer->createService(SERVICE_UUID);
    if (pService == nullptr) {
        // Serial.println("ERROR: Failed to create BLE service");
        return;
    }

    // Sound Characteristic 
    pSoundCharacteristic = pService->createCharacteristic(
        SOUND_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ| BLECharacteristic::PROPERTY_NOTIFY
    );

     // Check if sound characteristic creation was successful
    if (pSoundCharacteristic == nullptr) {
        Serial.println("ERROR: Failed to create sound characteristic");
        return;
    }

    // Dust Characteristic (Read Only)
    pDustCharacteristic = pService->createCharacteristic(
        DUST_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ| BLECharacteristic::PROPERTY_NOTIFY
    );

    // Check if dust characteristic creation was successful
    if (pDustCharacteristic == nullptr) {
        Serial.println("ERROR: Failed to create dust characteristic");
        return;
    }

     // Gas Characteristic (Read Only)
    pGasCharacteristic = pService->createCharacteristic(
        GAS_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ| BLECharacteristic::PROPERTY_NOTIFY
    );

    // Check if gas characteristic creation was successful
    if (pGasCharacteristic == nullptr) {
        Serial.println("ERROR: Failed to create gas characteristic");
        return;
    }


    // make initial values of chracteristics Message objects as well using Message class
    Message initialSoundMessage = Message(Message::SOUND_DATA_MESSAGE, -1.1f);  //negative initial value to know its initial
    std::string jsonSoundMessage = initialSoundMessage.getJsonMessage();
    pSoundCharacteristic->setValue(jsonSoundMessage.c_str());

    Message initialDustMessage = Message(Message::DUST_DATA_MESSAGE, -1.1f);
    std::string jsonDustMessage = initialDustMessage.getJsonMessage();
    pDustCharacteristic->setValue(jsonDustMessage.c_str());

    Message initialGasMessage = Message(Message::GAS_DATA_MESSAGE, -1.1f);
    std::string jsonGasMessage = initialGasMessage.getJsonMessage();
    pGasCharacteristic->setValue(jsonGasMessage.c_str());

    // Set initial values 
    if (jsonSoundMessage.length() > 0) {
        pSoundCharacteristic->setValue(jsonSoundMessage.c_str());
        // Serial.println("Set initial sound characteristic value");
    } else {
        // Fallback if JSON creation fails
        const char* fallbackJson = "{\"messageType\":\"SOUND_SENSOR_DATA\",\"data\":-1.1,\"timeStamp\":0}";
        pSoundCharacteristic->setValue(fallbackJson);
        // Serial.println("Set fallback sound characteristic value");
    }
    
    if (jsonDustMessage.length() > 0) {
        pDustCharacteristic->setValue(jsonDustMessage.c_str());
        // Serial.println("Set initial dust characteristic value");
    } else {
        // Fallback if JSON creation fails
        const char* fallbackJson = "{\"messageType\":\"DUST_SENSOR_DATA\",\"data\":-1.1,\"timeStamp\":0}";
        pDustCharacteristic->setValue(fallbackJson);
        // Serial.println("Set fallback dust characteristic value");
    }

     if (jsonGasMessage.length() > 0) {
        pGasCharacteristic->setValue(jsonGasMessage.c_str());
        // Serial.println("Set initial gas characteristic value");
    } else {
        // Fallback if JSON creation fails
        const char* fallbackJson = "{\"messageType\":\"GAS_SENSOR_DATA\",\"data\":-1.1,\"timeStamp\":0}";
        pDustCharacteristic->setValue(fallbackJson);
        // Serial.println("Set fallback gas characteristic value");
    }
  
    // sound sensor
    BLE2902* soundDescriptor = new BLE2902();
    if (soundDescriptor == nullptr) {
        Serial.println("ERROR: Failed to create sound descriptor");
        return;
    }
    
    //Necessary for app to pick up notifications
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
    
    //Necessary for app to pick up notifications
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

      // gas sensor
    BLE2902* gasDescriptor = new BLE2902();
    if (gasDescriptor == nullptr) {
        Serial.println("ERROR: Failed to create gas descriptor");
        return;
    }
    
    //Necessary for app to pick up notifications
    gasDescriptor->setNotifications(true);
    bool gasDescriptorAdded = true;
    try {
        pGasCharacteristic->addDescriptor(gasDescriptor);
    } catch (...) {
        gasDescriptorAdded = false;
        Serial.println("ERROR: Exception while adding gas descriptor");
    }
    
    if (!gasDescriptorAdded) {
        Serial.println("ERROR: Failed to add gas descriptor");
        delete gasDescriptor; // Prevent memory leak
        return;
    }
    
    // Set callbacks 
    pSoundCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pDustCharacteristic->setCallbacks(new CharacteristicCallbacks()); 
    pGasCharacteristic->setCallbacks(new CharacteristicCallbacks());    



    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);

    pAdvertising->setScanResponse(true);
    try {
      
        pAdvertising->setMinPreferred(0x12);   
    } catch (...) {
        Serial.println("WARNING: Could not set preferred connection parameters - library may not support it");
    }

    BLEDevice::startAdvertising();

    // Serial.println("\n=== BLE Setup Complete ===");
    // Serial.println("Device Name: SmartHat");
    // Serial.println("Service UUID: " + String(SERVICE_UUID));
    // Serial.println("Sound Characteristic UUID: " + String(SOUND_CHARACTERISTIC_UUID));
    // Serial.println("Dust Characteristic UUID: " + String(DUST_CHARACTERISTIC_UUID));
    // Serial.println("Gas Characteristic UUID: " + String(GAS_CHARACTERISTIC_UUID));
    // Serial.println("Waiting for connections...");
}

// Method to update sound level (using a float value)
void BleHandler::updateSoundLevel(float soundLevel) {

    if(deviceConnected){
      //create message from sound sensor and format in JSON
      Message soundMessage = Message(Message::SOUND_DATA_MESSAGE, soundLevel);
      std::string jsonMessage = soundMessage.getJsonMessage();

      //set the value of sound characteristic to JSON string so that android app can process
      pSoundCharacteristic->setValue(jsonMessage.c_str());
      pSoundCharacteristic->notify();

      //print for debug
    //   Serial.println("\nvalue of sound sensor characteristic sent to app: ");
    //   Serial.println(jsonMessage.c_str());  
    }
}

// Method to update dust level (using a float value)
void BleHandler::updateDustLevel(float dustLevel) {
    
    if(deviceConnected){
      //create message from dust sensor and format in JSON
      Message dustMessage = Message(Message::DUST_DATA_MESSAGE, dustLevel);
      std:: string jsonMessage=dustMessage.getJsonMessage();

      //set the vakue of the dust characteristic to the JSON string so that the android app can process it
      pDustCharacteristic->setValue(jsonMessage.c_str());
      pDustCharacteristic->notify();

      //print for debugging purposes
    //   Serial.println("\nvalue of dust sensor characteristic sent to app: ");
    //   Serial.println(jsonMessage.c_str());
    }
}

// Method to update gas level (using a float value)
void BleHandler::updateGasLevel(float gasLevel) {

    if(deviceConnected){
      //create message from gas sensor and format in JSON
      Message gasMessage = Message(Message::GAS_DATA_MESSAGE, gasLevel);
      std::string jsonMessage = gasMessage.getJsonMessage();

      //set the value of gas characteristic to JSON string so that android app can process
      pGasCharacteristic->setValue(jsonMessage.c_str());
      pGasCharacteristic->notify();

      //print for debug
    //   Serial.println("\nvalue of gas sensor characteristic sent to app: ");
    //   Serial.println(jsonMessage.c_str());  
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

// Getter for the Gas Characteristic
BLECharacteristic* BleHandler::getGasCharacteristic() {
    return pGasCharacteristic;
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

// Setter for the Gas Characteristic
void BleHandler::setGasCharacteristic(BLECharacteristic* gasCharacteristic) {
    pGasCharacteristic = gasCharacteristic;
}


bool BleHandler::isDeviceConnected() {
    return deviceConnected;
}



