#include "DustSensor.h"

DustSensor::DustSensor(int sensorPin, int ledPin, int adcMax, float vcc) {
    this->SENSOR_PIN = sensorPin;
    this->LED_PIN = ledPin;
    this->ADC_MAX = adcMax;
    this->VCC = vcc;
}

float DustSensor::getVoltage(int rawADC) {
    return (VCC / ADC_MAX) * rawADC;  // Convert ADC to voltage
}

float DustSensor::readDustSensor() {
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(280);
    
    int adc = analogRead(SENSOR_PIN);
    float v0 = getVoltage(adc);
    
    // Adjusted formula for 3.3V supply
    float density = ((v0 - 0.6) / 0.5) * 100;  // ug/m³

    digitalWrite(LED_PIN, HIGH);
    
    return max(density, 0.0f);  // Prevent negative values
}

void DustSensor::setUpDustSensor() {
    
    pinMode(LED_PIN, OUTPUT);
    delay(1000);
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