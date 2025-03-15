#include "BleHandler.h"
#include "NoiseSensor.h"
#include "DustSensor.h"
#include <ArduinoJson.h>
#include "Message.h"


BleHandler bleHandler;

//NoiseSensor noiseSensor(sensorPin, loudnessThreshold, numSamples);
NoiseSensor noiseSensor(34, 1.0, 20);

//DustSensor dustSensor(SENSOR_PIN, LED_PIN, ADC_MAX, VCC)
DustSensor dustSensor(35,23,4095,3.3);

void setup() {
    Serial.begin(115200);
    // SerialBT.begin("SmartHat");
    bleHandler.setUpBle();
    
    //Init noise sensor
    noiseSensor.begin();

    //init dust sensir
    dustSensor.setUpDustSensor();

     // Print the characteristic addresses

    analogReadResolution(12); // Set the resolution to 12 bits (0-4095)
}

void loop() {
    // Update the noise sensor readings
    noiseSensor.update();

    //print sound characteristic pointer for debug
    Serial.print("\nSound Characteristic: ");
    Serial.println((uint32_t)bleHandler.getSoundCharacteristic(), HEX);

    //print dust characteristic pointer for debug
    Serial.print("\nDust Characteristic: ");
    Serial.println((uint32_t)bleHandler.getDustCharacteristic(), HEX);

    // Collect sensor readings
    float dustSensorReading = dustSensor.readDustSensor();  
    float soundSensorReading = noiseSensor.getAverageVoltage();

    // update value of dust characteristic with new sensor reading
    bleHandler.updateDustLevel(dustSensorReading);
    
    // update value of sound characteristic with new sensor reading
    bleHandler.updateSoundLevel(soundSensorReading);
    

   
    delay(1000);
}




