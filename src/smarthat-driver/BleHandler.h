#ifndef BLEHANDLER_H
#define BLEHANDLER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

class BleHandler {
  private:
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pSoundCharacteristic;
    BLECharacteristic *pDustCharacteristic;
  
  public:
    // Constructor
    BleHandler();

    // BLE Setup
    void setUpBle();

    // Methods to update sensor values
    void updateSoundLevel(float soundLevel);
    void updateDustLevel(float dustLevel);

    // Getters
    BLEServer* getServer();
    BLEService* getService();
    BLECharacteristic* getSoundCharacteristic();
    BLECharacteristic* getDustCharacteristic();

    
    // Setters
    void setServer(BLEServer* server);
    void setService(BLEService* service);
    void setSoundCharacteristic(BLECharacteristic* soundCharacteristic);
    void setDustCharacteristic(BLECharacteristic* dustCharacteristic);

};

#endif
