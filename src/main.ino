//this is an example code , + loop to send sensor data as Json
#include <BluetoothSerial.h>
#include <ArduinoJSON.h>
#include "Message.h"

BluetoothSerial SerialBT;

float getDustSensorReading();
float getSoundSensorReading();

void setup() {
    Serial.begin(115200);
    SerialBT.begin("SmartHat");
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
    return 0.0f;
}

float getSoundSensorReading() {
    // Write sound sensor logic here
    return 0.0f;
}