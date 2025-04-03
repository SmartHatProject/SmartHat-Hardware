#ifndef BLEHANDLER_H
#define BLEHANDLER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// added forward declarations 
// class declared before referenced 
class ServerCallbacks;
class CharacteristicCallbacks;

class ServerCallbacks: public BLEServerCallbacks {
  private:
    bool* deviceConnected;
  public:
    ServerCallbacks(bool* connected);
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};


class BleHandler {
  private:
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pSoundCharacteristic;
    BLECharacteristic *pDustCharacteristic;
    BLECharacteristic *pGasCharacteristic;
    bool deviceConnected;
  
  public:
    // Constructor
    BleHandler();

    // BLE Setup
    void setUpBle();

    // Methods to update sensor values
    void updateSoundLevel(float soundLevel);
    void updateDustLevel(float dustLevel);
    void updateGasLevel(float gasLevel);

    // Getters
    BLEServer* getServer();
    BLEService* getService();
    BLECharacteristic* getSoundCharacteristic();
    BLECharacteristic* getDustCharacteristic();
    BLECharacteristic* getGasCharacteristic();
    bool isDeviceConnected();
    
    // Setters
    void setServer(BLEServer* server);
    void setService(BLEService* service);
    void setSoundCharacteristic(BLECharacteristic* soundCharacteristic);
    void setDustCharacteristic(BLECharacteristic* dustCharacteristic);
    void setGasCharacteristic(BLECharacteristic* gasCharacteristic);
};
};

#endif
