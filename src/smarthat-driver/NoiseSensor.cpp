#include "NoiseSensor.h"

NoiseSensor::NoiseSensor(int sensorPin, float loudnessThreshold, int numSamples)
    : _sensorPin(sensorPin), _loudnessThreshold(loudnessThreshold), _numSamples(numSamples),
      _currentVoltage(0.0), _averageVoltage(0.0), _exposureStartTime(0), _isAlertActive(false) {
    _samples = new float[_numSamples];
    _sampleIndex = 0;
}

NoiseSensor::~NoiseSensor() {
//free memoty
    if (_samples != nullptr) {
        delete[] _samples;
        _samples = nullptr;
    }
}


void NoiseSensor::begin() {
    Serial.begin(115200);
    analogReadResolution(12);  
    for (int i = 0; i < _numSamples; i++) {
        _samples[i] = 0.0;
    }
}

void NoiseSensor::update() {
    static unsigned long lastUpdateTime = 0;
    unsigned long currentTime = millis();
 
    if (currentTime - lastUpdateTime < 100) {
        return; 
    }
    lastUpdateTime = currentTime;
   
    _currentVoltage = readSensor();

    _samples[_sampleIndex] = _currentVoltage;
    _sampleIndex = (_sampleIndex + 1) % _numSamples;  

    _averageVoltage = calculateAverageVoltage();

    Serial.print("Average Voltage: ");
    Serial.println(_averageVoltage, 3);

    if (_averageVoltage >= _loudnessThreshold) {
      
        if (_exposureStartTime == 0) {
            _exposureStartTime = millis(); 
            Serial.println("Loud noise detected. Exposure timer started.");
        }

        unsigned long elapsedTime = (millis() - _exposureStartTime) / 1000;  

        Serial.print("Elapsed Time: ");
        Serial.print(elapsedTime);
        Serial.println(" s");
        if (elapsedTime >= 4) {
            triggerAlert();
            _exposureStartTime = 0; 
        }
    } else {
        if (_exposureStartTime != 0) {
            _exposureStartTime = 0; 
            Serial.println("Noise level dropped. Exposure timer reset.");
        }
    }
  
}

float NoiseSensor::readSensor() {
    int sensorValue = analogRead(_sensorPin);
    float voltage = (sensorValue / 4095.0) * 5.0;  //5v

    Serial.print("Raw Sensor Value: ");
    Serial.print(sensorValue);
    Serial.print(" | Voltage: ");
    Serial.println(voltage, 3);

    return voltage;
}

float NoiseSensor::calculateAverageVoltage() {
    float avgVoltage = 0.0;
    for (int i = 0; i < _numSamples; i++) {
        avgVoltage += _samples[i];
    }
    avgVoltage /= _numSamples;
    return avgVoltage;
}

// Trigger an alert
void NoiseSensor::triggerAlert() {
    
    Serial.println("****************************************");
    Serial.println("ALERT: Loud noise exposure for 10 seconds.");
    Serial.println("Put on ear protection or move to a quieter environment.");
    Serial.println("****************************************");
    _isAlertActive = true;
}

// Getters
int NoiseSensor::getSensorPin() const {
    return _sensorPin;
}

float NoiseSensor::getLoudnessThreshold() const {
    return _loudnessThreshold;
}

int NoiseSensor::getNumSamples() const {
    return _numSamples;
}

float NoiseSensor::getCurrentVoltage() const {
    return _currentVoltage;
}

float NoiseSensor::getAverageVoltage() const {
    return _averageVoltage;
}

unsigned long NoiseSensor::getExposureStartTime() const {
    return _exposureStartTime;
}

bool NoiseSensor::isAlertActive() const {
    return _isAlertActive;
}

// Setters
void NoiseSensor::setSensorPin(int pin) {
    _sensorPin = pin;
}

void NoiseSensor::setLoudnessThreshold(float threshold) {
    _loudnessThreshold = threshold;
}

void NoiseSensor::setNumSamples(int samples) {
    if (samples > 0) {
        _numSamples = samples;
        delete[] _samples;  
        _samples = new float[_numSamples];  
        _sampleIndex = 0; 
    }
}
