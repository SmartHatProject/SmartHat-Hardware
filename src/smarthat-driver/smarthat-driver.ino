#include "BleHandler.h"
#include "NoiseSensor.h"
#include "DustSensor.h"
#include <ArduinoJson.h>
#include "Message.h"

#define DUST_ALERT_THRESHOLD 50.0
#define SOUND_ALERT_THRESHOLD 85.0

//dust sensor constants
#define DUST_SENSOR_PIN 35
#define DUST_LED_PIN 23
#define DUST_ADC_MAX 4095
#define DUST_VCC 3.3

//noise sensor constants
#define NOISE_SENSOR_PIN 34
#define NOISE_CALIBRATION_DB 57.0
#define NOISE_PEAK_REF 373
#define NOISE_AVG_REF 363


bool lastConnectionStatus = false;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 1000; 


BleHandler bleHandler;

//NoiseSensor noiseSensor(sensorPin, loudnessThreshold, numSamples);
// NoiseSensor noiseSensor(NOISE_SENSOR_PIN, NOISE_CALIBRATION_DB, NOISE_PEAK_REF, NOISE_AVG_REF);

//DustSensor dustSensor(SENSOR_PIN, LED_PIN, ADC_MAX, VCC)
DustSensor dustSensor(DUST_SENSOR_PIN, DUST_LED_PIN, DUST_ADC_MAX, DUST_VCC);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\nset Serial");
    Serial.println("\n========== SmartHat Starting Up ==========");

    // SerialBT.begin("SmartHat");
    // bleHandler.setUpBle();
    
    //Init noise sensor
    // noiseSensor.begin();

    //init dust sensir
    Serial.println("\nsetting Dust Sensor");
    dustSensor.setUpDustSensor();
    Serial.println("\nsuccessfully set up dust sensor");


    analogReadResolution(12); // Set the resolution to 12 bits (0-4095)

    Serial.println("Setup complete, waiting for BLE connections...");
}

void loop() {

    // bool currentConnectionStatus = bleHandler.isDeviceConnected();

    // //print connection status for new connection or disconnection
    // if (currentConnectionStatus != lastConnectionStatus){
    //   if (currentConnectionStatus){
    //     Serial.println("\n========== CONNECTED ==========");
    //   }
    //   else{
    //     Serial.println("\n========== BLE CLIENT DISCONNECTED ==========");
    //   }
    // }
    // lastConnectionStatus = currentConnectionStatus;

    

    // //update BLE characteristics if it's been more than 1000 ms and the SmartHat is connected to a client
    // unsigned long currentTime = millis();
    // if (currentTime - lastUpdateTime >= UPDATE_INTERVAL){

    //   lastUpdateTime = currentTime;

    //   if(currentConnectionStatus){  

    //     // update value of dust characteristic with new sensor reading
    //     float dustSensorReading = dustSensor.readDustSensor();
    //     bleHandler.updateDustLevel(dustSensorReading);
    
    //     // update value of sound characteristic with new sensor reading
    //     noiseSensor.update();
    //     bleHandler.updateSoundLevel(noiseSensor.getPeakDB());
    //   }
    
    // }
    // yield();
  float dustSensorReading = dustSensor.readDustSensor();
  Serial.println("\n Dust sensor reading: ");
  Serial.println(dustSensorReading);


    delay(1000);
}




