#include <BluetoothSerial.h> // not for ble
 // use this instead
 //  #include <BLEDevice.h>
 //  #include <BLEServer.h>
 //  #include <BLEUtils.h>
//   #include <BLE2902.h>

//noise sensor:
#define SOUND_ENV_PIN 34  // Analog input for sound level
#define SOUND_GATE_PIN 27 // Digital input for sound detection
#define BASELINE_NOISE 500  // Baseline noise level (adjust based on quiet room readings)

//dust sensor:
#define DUST_SENSOR_PIN 35  // ADC1 Channel 7 (GPIO 35) Analog input for dust level


#include <ArduinoJson.h>
#include "Message.h"

BluetoothSerial SerialBT;

float getDustSensorReading();
float getSoundSensorReading();

void setup() {
    Serial.begin(115200);
    SerialBT.begin("SmartHat");
    pinMode(SOUND_GATE_PIN,INPUT);
    analogReadResolution(12); // Set the resolution to 12 bits (0-4095)
}

void loop() {
    // Collect data periodically
    float dustSensorReading = getDustSensorReading();  // Periodically update this dust reading
    float soundSensorReading = getSoundSensorReading();

    // Create and send dust sensor data message
    Message dustMessage(Message::DUST_DATA_MESSAGE, dustSensorReading);
    serializeJson(dustMessage.getJsonMessage(), SerialBT);

    // Create and send sound sensor data message
    Message soundMessage(Message::SOUND_DATA_MESSAGE, soundSensorReading);
    serializeJson(soundMessage.getJsonMessage(), SerialBT);

    SerialBT.println();
    delay(1000);
}

float getDustSensorReading() {
    // Write dust sensor logic here
    // Read values from the sensor
    int soundLevel = analogRead(SOUND_ENV_PIN);  // Get volume level (Envelope)
    int soundDetected = digitalRead(SOUND_GATE_PIN);  // Detect loud sound (Gate)

    // Prevent division by zero
    if (soundLevel <= BASELINE_NOISE) soundLevel = BASELINE_NOISE + 1;

    // Convert to dB using logarithmic scale
    float dB = 20.0 * log10((float)soundLevel / BASELINE_NOISE);

    // Print values to Serial Monitor
    Serial.print("\nSound Level (ENV): ");
    Serial.print(soundLevel);
    Serial.print("\n | Estimated dB: ");
    Serial.print(dB);
    Serial.print("\n dB | Sound Detected (GATE): ");
    Serial.print(soundDetected);
    return 0.0f;
}

float getSoundSensorReading() {
    // Write sound sensor logic here
    // Read the analog value from the sensor (0-4095)
    int sensorValue = analogRead(DUST_SENSOR_PIN);

    // Print the sensor value to the Serial Monitor
    Serial.print("\nSensor Value: ");
    Serial.println(sensorValue);
    return 0.0f;
}
