#include "BleHandler.h"
#include "NoiseSensor.h"
#include <ArduinoJson.h>
#include "Message.h"


BleHandler bleHandler;
NoiseSensor noiseSensor(34, 1.0, 20);
#define DUST_SENSOR_PIN 35  // ADC1 Channel 7 (GPIO 35) Analog input for dust level


bool lastConnectionStatus = false;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 1000; 

int testCounter = 0;

float getDustSensorReading();

void setup() {
    Serial.begin(115200);
    Serial.println("\n========== SmartHat Starting Up ==========");
    
    bleHandler.setUpBle();
    
    noiseSensor.begin();
    Serial.println("Noise sensor initialized");
    Serial.print("Sound Characteristic Address: ");
    Serial.println((uint32_t)bleHandler.getSoundCharacteristic(), HEX);

    Serial.print("Dust Characteristic Address: ");
    Serial.println((uint32_t)bleHandler.getDustCharacteristic(), HEX);

    analogReadResolution(12); 
    
    Serial.println("Setup complete, waiting for BLE connections...");
}

void loop() {

    bool currentConnectionStatus = bleHandler.isDeviceConnected();
    if (currentConnectionStatus != lastConnectionStatus) {
        if (currentConnectionStatus) {
            Serial.println("\n========== CONNECTED ==========");
            Serial.println("Starting to send test values...");
            
           
            testCounter = 100; 
    
            bleHandler.updateDustLevel(testCounter);
            bleHandler.updateSoundLevel(testCounter);
        } else {
            Serial.println("\n========== BLE CLIENT DISCONNECTED ==========");
        }
        lastConnectionStatus = currentConnectionStatus;
    }
    
    noiseSensor.update();

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        lastUpdateTime = currentTime;
        
    
        float dustSensorReading = getDustSensorReading();  
        float soundSensorReading = noiseSensor.getAverageVoltage();
        
        testCounter++;
        if (testCounter > 200) testCounter = 100; 
        
        float testValue = (float)testCounter;
        
        Serial.println("\n========== Sensor Readings ==========");
        Serial.print("Dust: ");
        Serial.println(dustSensorReading, 2);
        Serial.print("Sound: ");
        Serial.println(soundSensorReading, 2);
        Serial.print("Test Value: ");
        Serial.println(testValue);
     
        if (currentConnectionStatus) {
            bleHandler.updateDustLevel(testValue);
            bleHandler.updateSoundLevel(testValue);
            
           
            if (testCounter % 5 == 0) {
                Serial.println("\n========== Sending Simplified Format ==========");
                char simpleJson[100];
                
                sprintf(simpleJson, "{\"messageType\":\"SOUND_SENSOR_DATA\",\"data\":%.2f,\"timeStamp\":%lu}", 
                    testValue, millis());
                Serial.println("Sound JSON: " + String(simpleJson));
                bleHandler.getSoundCharacteristic()->setValue(simpleJson);
                bleHandler.getSoundCharacteristic()->notify();
                
                sprintf(simpleJson, "{\"messageType\":\"DUST_SENSOR_DATA\",\"data\":%.2f,\"timeStamp\":%lu}", 
                    testValue, millis());
                Serial.println("Dust JSON: " + String(simpleJson));
                bleHandler.getDustCharacteristic()->setValue(simpleJson);
                bleHandler.getDustCharacteristic()->notify();
            }
        }
    }
    
  // non blocking 
    yield();
}

float getDustSensorReading() {
    int sensorValue = analogRead(DUST_SENSOR_PIN);

    
    Serial.print("Dust Sensor Raw Value: ");
    Serial.println(sensorValue);
    

    static float baseValue = 7.23f;
    baseValue += (random(100) - 50) / 100.0; 
    
    return baseValue;
}




