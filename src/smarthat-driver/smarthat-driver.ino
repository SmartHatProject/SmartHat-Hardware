#include "BleHandler.h"
#include "NoiseSensor.h"
#include <ArduinoJson.h>
#include "Message.h"


BleHandler bleHandler;

//NoiseSensor noiseSensor(sensorPin, loudnessThreshold, numSamples);
NoiseSensor noiseSensor(34, 1.0, 20);

//dust sensor:
#define DUST_SENSOR_PIN 35  // ADC1 Channel 7 (GPIO 35) Analog input for dust level


bool lastConnectionStatus = false;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 1000; // Update interval in milliseconds

// test value counter
int testCounter = 0;

float getDustSensorReading();

void setup() {
    Serial.begin(115200);
    Serial.println("\n========== SmartHat Starting Up ==========");
    
    // Initialize BLE
    bleHandler.setUpBle();
    
    // Initialize noise sensor
    noiseSensor.begin();
    Serial.println("Noise sensor initialized");

    // Print the characteristic addresses for debug
    Serial.print("Sound Characteristic Address: ");
    Serial.println((uint32_t)bleHandler.getSoundCharacteristic(), HEX);

    Serial.print("Dust Characteristic Address: ");
    Serial.println((uint32_t)bleHandler.getDustCharacteristic(), HEX);

    analogReadResolution(12); // Set the resolution to 12 bits (0-4095)
    
    Serial.println("Setup complete, waiting for BLE connections...");
}

void loop() {
    // Check if connection status changed
    bool currentConnectionStatus = bleHandler.isDeviceConnected();
    if (currentConnectionStatus != lastConnectionStatus) {
        if (currentConnectionStatus) {
            Serial.println("\n========== BLE CLIENT CONNECTED ==========");
            Serial.println("Starting to send test values...");
            
           
            testCounter = 100; 
            
            // Send initial test values
            bleHandler.updateDustLevel(testCounter);
            bleHandler.updateSoundLevel(testCounter);
        } else {
            Serial.println("\n========== BLE CLIENT DISCONNECTED ==========");
        }
        lastConnectionStatus = currentConnectionStatus;
    }
    
    // Update the noise sensor readings
    noiseSensor.update();
    
    // update intervals
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        lastUpdateTime = currentTime;
        
        // Collect sensor readings
        float dustSensorReading = getDustSensorReading();  
        float soundSensorReading = noiseSensor.getAverageVoltage();
        
        // Add a constantly increasing test value for debugging
        testCounter++;
        if (testCounter > 200) testCounter = 100;  // Keep in a reasonable range
        
        // Use testCounter as our value for debugging
        float testValue = (float)testCounter;
        
        Serial.println("\n========== Sensor Readings ==========");
        Serial.print("Dust: ");
        Serial.println(dustSensorReading, 2);
        Serial.print("Sound: ");
        Serial.println(soundSensorReading, 2);
        Serial.print("Test Value: ");
        Serial.println(testValue);
        
        // Update BLE characteristics with test value
        if (currentConnectionStatus) {
            // Send both regular and test values
            bleHandler.updateDustLevel(testValue);
            bleHandler.updateSoundLevel(testValue);
            
           
            if (testCounter % 5 == 0) {
                Serial.println("\n========== Sending Simplified Format ==========");
                char simpleJson[100];
                
                // Create proper JSON format for sound data
                sprintf(simpleJson, "{\"messageType\":\"SOUND_SENSOR_DATA\",\"data\":%.2f,\"timeStamp\":%lu}", 
                    testValue, millis());
                Serial.println("Sound JSON: " + String(simpleJson));
                bleHandler.getSoundCharacteristic()->setValue(simpleJson);
                bleHandler.getSoundCharacteristic()->notify();
                
                // Create proper JSON format for dust data
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
    // Read the analog value from the sensor (0-4095)
    int sensorValue = analogRead(DUST_SENSOR_PIN);

    
    Serial.print("Dust Sensor Raw Value: ");
    Serial.println(sensorValue);
    

    static float baseValue = 7.23f;
    baseValue += (random(100) - 50) / 100.0; 
    
    return baseValue;
}




