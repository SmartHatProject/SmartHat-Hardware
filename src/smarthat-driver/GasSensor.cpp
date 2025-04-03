#include "GasSensor.h"
#include <Math.h>

GasSensor::GasSensor(int pin, float loadResistance, int numSamples)
    : _pin(pin), _rl(loadResistance), _numSamples(numSamples), _r0(0.0) {}

void GasSensor::begin() {
    Serial.begin(115200);
    Serial.println("MQ135 Sensor on ESP32 - Calibrating R0");

    delay(10000);  // Sensor warm-up
    _r0 = calibrateR0();
    Serial.print("Calibrated R0 (in clean air): ");
    Serial.println(_r0);
}

float GasSensor::calibrateR0() {
    float analogAvg = getAverageAnalog();
    float rs = calculateRs(analogAvg);
    return rs;
}

float GasSensor::readPPM() {
    float analogAvg = getAverageAnalog();
    float rs = calculateRs(analogAvg);
    float ppm = getPPM(rs, _r0);

    Serial.print("Analog Avg: ");
    Serial.print(analogAvg);
    Serial.print(" | Rs: ");
    Serial.print(rs);
    Serial.print(" kΩ | PPM: ");
    Serial.println(ppm);

    return ppm;
}

float GasSensor::getAverageAnalog() {
    long sum = 0;
    for (int i = 0; i < _numSamples; i++) {
        sum += analogRead(_pin);
        delay(10);
    }
    return (float)sum / _numSamples;
}

float GasSensor::calculateRs(float analogValue) {
    float voltage = analogValue * (3.3 / 4095.0);  // 12-bit ADC
    return (3.3 - voltage) * _rl / voltage;
}

float GasSensor::getPPM(float rs, float r0) {
    float ratio = rs / r0;
    float a = -2.769;
    float b = 2.602;
    float log_ppm = a * log10(ratio) + b;
    return pow(10, log_ppm);
}
