#include "BleHandler.h"
#include "NoiseSensor.h"
#include "DustSensor.h"
#include <ArduinoJson.h>
#include "Message.h"

#define DUST_ALERT_THRESHOLD 50.0
#define SOUND_ALERT_THRESHOLD 85.0

bool lastConnectionStatus = false;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 1000; 


BleHandler bleHandler;

//NoiseSensor noiseSensor(sensorPin, loudnessThreshold, numSamples);
NoiseSensor noiseSensor(34, 1.0, 20);

//DustSensor dustSensor(SENSOR_PIN, LED_PIN, ADC_MAX, VCC)
DustSensor dustSensor(35,23,4095,3.3);

void setup() {
    Serial.begin(115200);
    Serial.println("\n========== SmartHat Starting Up ==========");

    // SerialBT.begin("SmartHat");
    bleHandler.setUpBle();
    
    //Init noise sensor
    noiseSensor.begin();

    //init dust sensir
    dustSensor.setUpDustSensor();

    analogReadResolution(12); // Set the resolution to 12 bits (0-4095)

    Serial.println("Setup complete, waiting for BLE connections...");
}

void loop() {

    bool currentConnectionStatus = bleHandler.isDeviceConnected();

    //print connection status for new connection or disconnection
    if (currentConnectionStatus != lastConnectionStatus){
      if (currentConnectionStatus){
        Serial.println("\n========== CONNECTED ==========");
      }
      else{
        Serial.println("\n========== BLE CLIENT DISCONNECTED ==========");
      }
    }
    lastConnectionStatus = currentConnectionStatus;

    noiseSensor.update();

    //update BLE characteristics if it's been more than 1000 ms and the SmartHat is connected to a client
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL){

      lastUpdateTime = currentTime;

      if(currentConnectionStatus){
        // Collect sensor readings
        float dustSensorReading = dustSensor.readDustSensor();  
        float soundSensorReading = noiseSensor.getAverageVoltage();

        // update value of dust characteristic with new sensor reading
        bleHandler.updateDustLevel(dustSensorReading);
    
        // update value of sound characteristic with new sensor reading
        bleHandler.updateSoundLevel(soundSensorReading);
      }
    
    }
    yield();
    delay(1000);
}




