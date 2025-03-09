#include "BleHandler.h"
#include "NoiseSensor.h"
#include <ArduinoJson.h>
#include "Message.h"


BleHandler bleHandler;

//NoiseSensor noiseSensor(sensorPin, loudnessThreshold, numSamples);
NoiseSensor noiseSensor(34, 1.0, 20);

//dust sensor:
#define DUST_SENSOR_PIN 35  // ADC1 Channel 7 (GPIO 35) Analog input for dust level


float getDustSensorReading();

void setup() {
    Serial.begin(115200);
    // SerialBT.begin("SmartHat");
    bleHandler.setUpBle();
    
    //Init noise sensor
    noiseSensor.begin();

     // Print the characteristic addresses

    analogReadResolution(12); // Set the resolution to 12 bits (0-4095)
}

void loop() {
    // Update the noise sensor readings
    noiseSensor.update();

    Serial.print("Sound Characteristic: ");
    Serial.println((uint32_t)bleHandler.getSoundCharacteristic(), HEX);

    Serial.print("Dust Characteristic: ");
    Serial.println((uint32_t)bleHandler.getDustCharacteristic(), HEX);

    // Collect data periodically
    float dustSensorReading = getDustSensorReading();  // Periodically update this dust reading
    float soundSensorReading = noiseSensor.getAverageVoltage();

    // Create and send dust sensor data message
    bleHandler.updateDustLevel(dustSensorReading);
    Message dustMessage(Message::DUST_DATA_MESSAGE, dustSensorReading);
    // serializeJson(dustMessage.getJsonMessage(), SerialBT);

    // Create and send sound sensor data message
    bleHandler.updateSoundLevel(soundSensorReading);
    Message soundMessage(Message::SOUND_DATA_MESSAGE, soundSensorReading);
    // serializeJson(soundMessage.getJsonMessage(), SerialBT);

    // Read Sound Characteristic
    String soundRawValue = bleHandler.getSoundCharacteristic()->getValue();
    float soundValue;
    if (soundRawValue.length() == sizeof(float)) {
        memcpy(&soundValue, soundRawValue.c_str(), sizeof(float));
    } else {
        soundValue = -1.0f; // Error case
    }

    String dustRawValue = bleHandler.getDustCharacteristic()->getValue();
    float dustValue;
    if (dustRawValue.length() == sizeof(float)) {
        memcpy(&dustValue, dustRawValue.c_str(), sizeof(float));
    } else {
        dustValue = -1.0f; // Error case
    }

    Serial.print("BLE Sound Value: ");
    Serial.println(soundValue, 2); // Print with 2 decimal places
    Serial.print("BLE Dust Value: ");
    Serial.println(dustValue, 2);

    // SerialBT.println();
    delay(50);
}

float getDustSensorReading() {
    // Write sound sensor logic here
    // Read the analog value from the sensor (0-4095)
    int sensorValue = analogRead(DUST_SENSOR_PIN);

    // Print the sensor value to the Serial Monitor
    Serial.print("\nSensor Value: ");
    Serial.println(sensorValue);
    return 0.0f;
}




