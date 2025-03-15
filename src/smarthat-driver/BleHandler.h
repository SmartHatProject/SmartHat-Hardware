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
    bool deviceConnected;
  
  public:
  
    BleHandler();

    void setUpBle();

    void updateSoundLevel(float soundLevel);
    void updateDustLevel(float dustLevel);

    // Getters
    BLEServer* getServer();
    BLEService* getService();
    BLECharacteristic* getSoundCharacteristic();
    BLECharacteristic* getDustCharacteristic();
    bool isDeviceConnected();
    
    // Setters
    void setServer(BLEServer* server);
    void setService(BLEService* service);
    void setSoundCharacteristic(BLECharacteristic* soundCharacteristic);
    void setDustCharacteristic(BLECharacteristic* dustCharacteristic);
};

#endif
