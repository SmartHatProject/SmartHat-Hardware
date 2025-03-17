#include "BleHandler.h"
#include "NoiseSensor.h"
#include "DustSensor.h"
#include <ArduinoJson.h>
#include "Message.h"

#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define SOUND_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define DUST_CHARACTERISTIC_UUID  "dcba4321-8765-4321-8765-654321fedcba"

BleHandler bleHandler;
NoiseSensor noiseSensor(34, 1.0, 20);

#define DUST_SENSOR_PIN 35
#define DUST_SENSOR_LED_PIN 25

DustSensor dustSensor(DUST_SENSOR_PIN, DUST_SENSOR_LED_PIN, 4095, 3.3);

#define DUST_ALERT_THRESHOLD 50.0
#define SOUND_ALERT_THRESHOLD 85.0

bool lastConnectionStatus = false;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 1000; 

float getDustSensorReading();

void setup() {
    Serial.begin(115200);
    Serial.println("\n========== SmartHat Starting Up ==========");
    
    bleHandler.setUpBle();
    
    noiseSensor.begin();
    Serial.println("Noise sensor initialized");
    
    dustSensor.setUpDustSensor();
    
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
            Serial.println("Starting to send sensor data...");
            
            Serial.println("Taking initial sensor readings for immediate transmission");
            
            float dustReading = getDustSensorReading();
            float soundReading = noiseSensor.getAverageVoltage();
            
            Serial.println("\n========== Initial Sensor Readings ==========");
            Serial.print("Initial Dust Reading: ");
            Serial.print(dustReading, 2);
            Serial.println(" µg/m³");
            Serial.print("Initial Sound Reading: ");
            Serial.print(soundReading, 2);
            Serial.println(" dB");
            
            Serial.println("Sending initial readings via BLE...");
            bleHandler.updateDustLevel(dustReading);
            bleHandler.updateSoundLevel(soundReading);
            Serial.println("Initial readings sent successfully");
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
        
        Serial.println("\n========== Periodic Sensor Readings ==========");
        Serial.print("Dust: ");
        Serial.print(dustSensorReading, 2);
        Serial.println(" µg/m³");
        Serial.print("Sound: ");
        Serial.print(soundSensorReading, 2);
        Serial.println(" dB");
        
        if (dustSensorReading > DUST_ALERT_THRESHOLD) {
            Serial.println("\n*******************************************");
            Serial.println("* ALERT: HIGH DUST LEVEL DETECTED! *");
            Serial.println("* Current level: " + String(dustSensorReading, 1) + " μg/m³");
            Serial.println("* Threshold: " + String(DUST_ALERT_THRESHOLD, 1) + " μg/m³");
            Serial.println("* Action: Consider respiratory protection");
            Serial.println("*******************************************\n");
        }
        
        if (soundSensorReading > SOUND_ALERT_THRESHOLD) {
            Serial.println("\n*******************************************");
            Serial.println("* ALERT: DANGEROUS SOUND LEVEL DETECTED! *");
            Serial.println("* Current level: " + String(soundSensorReading, 1) + " dB");
            Serial.println("* Prolonged exposure may cause hearing damage");
            Serial.println("* Action: Put on ear protection immediately");
            Serial.println("*******************************************\n");
        }
     
        if (currentConnectionStatus) {
            Serial.println("Sending periodic readings via BLE...");
            bleHandler.updateDustLevel(dustSensorReading);
            bleHandler.updateSoundLevel(soundSensorReading);
            Serial.println("Periodic readings sent successfully");
        }
    }
    
    yield();
}

float getDustSensorReading() {
    const int numReadings = 5;
    float sum = 0.0;
    
    for (int i = 0; i < numReadings; i++) {
        float density = dustSensor.readDustSensor();
        sum += density;
        delay(10);
    }
    
    float avgDensity = sum / numReadings;
    
    Serial.print("Dust Sensor Average Reading: ");
    Serial.print(avgDensity, 1);
    Serial.println(" μg/m³");
    
    if (avgDensity < 0) avgDensity = 0;
    if (avgDensity > 1000) avgDensity = 1000;
    
    return avgDensity;
}