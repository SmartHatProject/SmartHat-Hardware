#ifndef NOISESENSOR_H
#define NOISESENSOR_H

#include <Arduino.h>

class NoiseSensor {
public:
    // Constructor
    NoiseSensor(int sensorPin, float loudnessThreshold, int numSamples);

    // Initialize the sensor
    void begin();

    // Main loop function to be called repeatedly
    void update();

    // Getters
    int getSensorPin() const;                  // Get the sensor pin
    float getLoudnessThreshold() const;        // Get the loudness threshold
    int getNumSamples() const;                 // Get the number of samples
    float getCurrentVoltage() const;           // Get the current voltage reading
    float getAverageVoltage() const;           // Get the average voltage
    unsigned long getExposureStartTime() const; // Get the exposure start time
    bool isAlertActive() const;                // Check if an alert is active

    // Setters
    void setSensorPin(int pin);                // Set the sensor pin
    void setLoudnessThreshold(float threshold); // Set the loudness threshold
    void setNumSamples(int samples);           // Set the number of samples

private:
    // Pin configuration
    int _sensorPin;

    // Loudness threshold
    float _loudnessThreshold;

    // Averaging parameters
    int _numSamples;
    float* _samples;
    int _sampleIndex;

    // Current voltage reading
    float _currentVoltage;

    // Average voltage
    float _averageVoltage;

    // Exposure tracking
    unsigned long _exposureStartTime;
    bool _isAlertActive;

    // Read the sensor and calculate voltage
    float readSensor();

    // Calculate the average voltage
    float calculateAverageVoltage();

    // Trigger an alert
    void triggerAlert();
};

#endif