#include "NoiseSensor.h"



NoiseSensor::NoiseSensor(int sensorPin, float loudnessThreshold, int numSamples)
    : _sensorPin(sensorPin), _loudnessThreshold(loudnessThreshold), _numSamples(numSamples),
      _currentVoltage(0.0), _averageVoltage(0.0), _exposureStartTime(0), _isAlertActive(false) {
    _samples = new float[_numSamples];
    _sampleIndex = 0;
}

NoiseSensor::~NoiseSensor() {
    // Free memory
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

    Serial.print("Average Sound Level (dB): ");
    Serial.println(_averageVoltage, 1);

    
    const float SOUND_ALERT_THRESHOLD = 85.0;
    
    if (_averageVoltage >= SOUND_ALERT_THRESHOLD) {
        if (_exposureStartTime == 0) {
            _exposureStartTime = millis(); 
            Serial.println("High sound level detected! Starting exposure timer.");
            Serial.print("Current level (dB): ");
            Serial.print(_averageVoltage, 1);
            Serial.print(" | Threshold (dB): ");
            Serial.println(SOUND_ALERT_THRESHOLD, 1);
        }

        unsigned long elapsedTime = (millis() - _exposureStartTime) / 1000;  

        Serial.print("High sound exposure time: ");
        Serial.print(elapsedTime);
        Serial.println("s");
        
        // After 4 seconds of continuous exposure, trigger alert
        if (elapsedTime >= 4) {
            triggerAlert();
            _exposureStartTime = 0; 
        }
    } else {
        if (_exposureStartTime != 0) {
            _exposureStartTime = 0; 
            Serial.println("Sound level dropped below threshold. Exposure timer reset.");
        }
    }
}


float NoiseSensor::readSensor() {
   
    const int numReadings = 5;
    float sum = 0.0;
    int validReadings = 0;
    
    Serial.println("Sound sensor readings:");
    for (int i = 0; i < numReadings; i++) {
        int sensorValue = analogRead(_sensorPin);
        float voltage = (sensorValue / 4095.0) * 3.3;  // Using 3.3V reference for ESP32
        
        // Log raw values
        Serial.print("  Reading ");
        Serial.print(i);
        Serial.print(": ADC=");
        Serial.print(sensorValue);
        Serial.print(", Voltage=");
        Serial.print(voltage, 3);
        Serial.print("V");
        
        
        if (voltage > 0.001 && voltage < 3.3) {
            sum += voltage;
            validReadings++;
            Serial.println(" (valid)");
        } else {
            Serial.println(" (invalid - outside expected voltage range)");
        }
        
        delay(1); // Small delay between readings
    }
    
   
    if (validReadings == 0) {
        Serial.println("[WARN] No valid voltage readings from sound sensor");
        return 30.0; 
    }
    
    float avgVoltage = sum / validReadings;
    
    if (avgVoltage <= 0.001) {
        Serial.println("[WARN] Very low voltage detected, using minimum value");
        avgVoltage = 0.001; 
    }
    
    float dbValue = 20.0 * log10(avgVoltage / 0.01) + 40.0; 
    
   
    if (dbValue < 30.0) {
        Serial.println("[WARN] Calculated dB value below minimum, clamping to 30.0 dB");
        dbValue = 30.0; 
    }
    if (dbValue > 130.0) {
        Serial.println("[WARN] Calculated dB value above maximum, clamping to 130.0 dB");
        dbValue = 130.0; 
    }
    
    Serial.print("Sound Sensor Summary: Valid readings=");
    Serial.print(validReadings);
    Serial.print("/");
    Serial.print(numReadings);
    Serial.print(", Avg Voltage=");
    Serial.print(avgVoltage, 3);
    Serial.print("V, Calculated dB=");
    Serial.println(dbValue, 1);

    return dbValue;
}


float NoiseSensor::calculateAverageVoltage() {
    // Skip any invalid samples
    int validSamples = 0;
    float sum = 0.0;
    
    Serial.println("Sound samples analysis:");
    for (int i = 0; i < _numSamples; i++) {
        Serial.print("  Sample ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(_samples[i], 1);
        Serial.print(" dB");
        
        if (_samples[i] >= 30.0 && _samples[i] <= 130.0) { // Valid dB range
            sum += _samples[i];
            validSamples++;
            Serial.println(" (valid)");
        } else {
            Serial.println(" (outside valid range 30-130 dB)");
        }
    }
    
    Serial.print("Valid samples: ");
    Serial.print(validSamples);
    Serial.print("/");
    Serial.println(_numSamples);
    
    // If no valid samples, return a reasonable default
    if (validSamples == 0) {
        Serial.println("[WARN] No valid sound samples detected, using default value of 30.0 dB");
        Serial.println("       This may indicate a sensor issue or very quiet environment");
        return 30.0; // Minimum valid value in dB range
    }
    
    float average = sum / validSamples;
    Serial.print("Sound level average: ");
    Serial.print(average, 1);
    Serial.println(" dB");
    
    return average;
}
//alert trigger
void NoiseSensor::triggerAlert() {
    Serial.println("\n*******************************************");
    Serial.println("* ALERT: DANGEROUS SOUND LEVEL DETECTED! *");
    Serial.println("* Current level: " + String(_averageVoltage, 1) + " dB");
    Serial.println("* Prolonged exposure may cause hearing damage");
    Serial.println("* Action: Put on ear protection immediately");
    Serial.println("*******************************************\n");
    
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
