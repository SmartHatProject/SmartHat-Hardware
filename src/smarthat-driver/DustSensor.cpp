#include "DustSensor.h"



DustSensor::DustSensor(int sensorPin, int ledPin, int adcMax, float vcc) {
    this->SENSOR_PIN = sensorPin;
    this->LED_PIN = ledPin;
    this->ADC_MAX = adcMax;
    this->VCC = vcc;
}

float DustSensor::getVoltage(int rawADC) {
   
    return (VCC / ADC_MAX) * rawADC;
}

float DustSensor::readDustSensor() {
   
    digitalWrite(LED_PIN, LOW);
    
   
    delayMicroseconds(280);
    
    // Read the analog value
    int adc = analogRead(SENSOR_PIN);
    
    // Convert to voltage
    float v0 = getVoltage(adc);
    
   
    Serial.print("Dust Sensor Raw ADC: ");
    Serial.print(adc);
    Serial.print(" | Voltage: ");
    Serial.print(v0, 3);
    Serial.println("V");
    
    // Check if voltage is below the detection threshold
    if (v0 < 0.6f) {
        Serial.println("[WARN] Dust sensor voltage below detection threshold: " + String(v0, 3) + "V");
        Serial.println("       This may indicate very clean air or a sensor issue");
        // We'll still calculate the density, but it will be clamped to 0
    }
    
   
    float density = ((v0 - 0.6) / 0.5) * 100.0;  
    
    
    digitalWrite(LED_PIN, HIGH);
    
  
    float finalDensity = max(density, 0.0f);
    
    
    Serial.print("Calculated Dust Density: ");
    Serial.print(finalDensity, 1);
    Serial.println(" µg/m³");
    
    return finalDensity;
}

void DustSensor::setUpDustSensor() {
    // Configure the LED pin as an output
    pinMode(LED_PIN, OUTPUT);
    
    digitalWrite(LED_PIN, HIGH);
    
   
    delay(1000);
    
    // Log initialization
    Serial.print("Dust sensor initialized on pin ");
    Serial.print(SENSOR_PIN);
    Serial.print(" with LED control on pin ");
    Serial.println(LED_PIN);
}

// Getters
int DustSensor::getSensorPin() const {
    return SENSOR_PIN;
}

int DustSensor::getLedPin() const {
    return LED_PIN;
}

int DustSensor::getAdcMax() const {
    return ADC_MAX;
}

float DustSensor::getVcc() const {
    return VCC;
}

// Setters
void DustSensor::setSensorPin(int sensorPin) {
    SENSOR_PIN = sensorPin;
}

void DustSensor::setLedPin(int ledPin) {
    LED_PIN = ledPin;
}

void DustSensor::setAdcMax(int adcMax) {
    ADC_MAX = adcMax;
}

void DustSensor::setVcc(float vcc) {
    VCC = vcc;
} 