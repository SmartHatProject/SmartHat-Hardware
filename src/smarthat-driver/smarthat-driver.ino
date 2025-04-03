#include "BleHandler.h"
#include "NoiseSensor.h"
#include "DustSensor.h"
#include "GasSensor.h"
#include <ArduinoJson.h>
#include "Message.h"

#define DUST_ALERT_THRESHOLD 50.0
#define SOUND_ALERT_THRESHOLD 85.0
#define GAS_ALERT_THRESHOLD 1000.0

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

//gas sensor constants
#define GAS_SENSOR_PIN 32
#define GAS_LOAD_RESISTANCE 10.0
#define GAS_NUM_SAMPLES 100


bool lastConnectionStatus = false;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 1000; 


BleHandler bleHandler;

//NoiseSensor noiseSensor(sensorPin, loudnessThreshold, numSamples);
NoiseSensor noiseSensor(NOISE_SENSOR_PIN, NOISE_CALIBRATION_DB, NOISE_PEAK_REF, NOISE_AVG_REF);

//GasSensor gasSensor(sensorPin,load resistance, numSamples);
GasSensor gasSensor(GAS_SENSOR_PIN, GAS_LOAD_RESISTANCE, GAS_NUM_SAMPLES);

//DustSensor dustSensor(SENSOR_PIN, LED_PIN, ADC_MAX, VCC)
DustSensor dustSensor(DUST_SENSOR_PIN, DUST_LED_PIN, DUST_ADC_MAX, DUST_VCC);

void setup() {
    Serial.begin(115200);
    Serial.println("\n========== SmartHat Starting Up ==========");

    // SerialBT.begin("SmartHat");
    bleHandler.setUpBle();
    
    //Init noise sensor
    noiseSensor.begin();

    //Init noise sensor
    gasSensor.begin();

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

    

    //update BLE characteristics if it's been more than 1000 ms and the SmartHat is connected to a client
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL){

      lastUpdateTime = currentTime;

      if(currentConnectionStatus){  

        // update value of dust characteristic with new sensor reading
        float dustSensorReading = dustSensor.readDustSensor();
        bleHandler.updateDustLevel(dustSensorReading);
    
        // update value of sound characteristic with new sensor reading
        noiseSensor.update();
        bleHandler.updateSoundLevel(noiseSensor.getPeakDB());

        // update value of gas characteristic with new sensor reading
        float gasSensorReading = gasSensor.readGasSensor();
        bleHandler.updateGasLevel(gasSensorReading);
      }
    
    }
    yield();
    delay(1000);
}




