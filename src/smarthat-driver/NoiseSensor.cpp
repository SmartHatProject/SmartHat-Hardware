#include "NoiseSensor.h"

// Constructor
NoiseSensor::NoiseSensor(int sensorPin, float loudnessThreshold, int numSamples)
    : _sensorPin(sensorPin), _loudnessThreshold(loudnessThreshold), _numSamples(numSamples),
      _currentVoltage(0.0), _averageVoltage(0.0), _exposureStartTime(0), _isAlertActive(false) {
    _samples = new float[_numSamples];
    _sampleIndex = 0;
}

// Initialize the sensor
void NoiseSensor::begin() {
    // Initialize serial communication
    Serial.begin(115200);

    // Configure the analog input pin
    analogReadResolution(12);  // Set ADC resolution to 12 bits

    // Initialize the samples array
    for (int i = 0; i < _numSamples; i++) {
        _samples[i] = 0.0;
    }
}

// Main loop function to be called repeatedly
void NoiseSensor::update() {
    // Read the sensor and calculate voltage
    _currentVoltage = readSensor();

    // Add the voltage to the samples array
    _samples[_sampleIndex] = _currentVoltage;
    _sampleIndex = (_sampleIndex + 1) % _numSamples;  // Wrap around the array

    // Calculate the average voltage over the last 2 seconds (20 samples × 100ms)
    _averageVoltage = calculateAverageVoltage();

    // Debug: Print average voltage
    Serial.print("Average Voltage: ");
    Serial.println(_averageVoltage, 3);

    // Check if the average voltage exceeds the loudness threshold
    if (_averageVoltage >= _loudnessThreshold) {
        // Start or continue exposure timer
        if (_exposureStartTime == 0) {
            _exposureStartTime = millis();  // Start the timer
            Serial.println("Loud noise detected. Exposure timer started.");
        }

        // Calculate elapsed time
        unsigned long elapsedTime = (millis() - _exposureStartTime) / 1000;  // Convert to seconds

        // Debugging: Print elapsed time
        Serial.print("Elapsed Time: ");
        Serial.print(elapsedTime);
        Serial.println(" s");

        // Trigger an alert if the exposure time exceeds 10 seconds
        if (elapsedTime >= 4) {
            triggerAlert();
            _exposureStartTime = 0;  // Reset the timer
        }
    } else {
        // Reset the timer if the noise level drops below the threshold
        if (_exposureStartTime != 0) {
            _exposureStartTime = 0;  // Reset the timer
            Serial.println("Noise level dropped. Exposure timer reset.");
        }
    }

    // Wait for 100ms before the next reading (20 samples × 100ms = 2-second window)
    delay(100);
}

// Read the sensor and calculate voltage
float NoiseSensor::readSensor() {
    int sensorValue = analogRead(_sensorPin);
    float voltage = (sensorValue / 4095.0) * 5.0;  // 5V supply

    Serial.print("Raw Sensor Value: ");
    Serial.print(sensorValue);
    Serial.print(" | Voltage: ");
    Serial.println(voltage, 3);

    return voltage;
}

// Calculate the average voltage
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