#ifndef NOISESENSOR_H
#define NOISESENSOR_H

#include <Arduino.h>

class NoiseSensor {
public:

    NoiseSensor(int sensorPin, float loudnessThreshold, int numSamples);
    ~NoiseSensor();

    void begin();

    void update();

    // Getters
    int getSensorPin() const;                  
    float getLoudnessThreshold() const;        
    int getNumSamples() const;                 
    float getCurrentVoltage() const;           
    float getAverageVoltage() const;         
    unsigned long getExposureStartTime() const; 
    bool isAlertActive() const;                

    // Setters
    void setSensorPin(int pin);                
    void setLoudnessThreshold(float threshold);
    void setNumSamples(int samples);           

private:
    
    int _sensorPin;
    float _loudnessThreshold;
    int _numSamples;
    float* _samples;
    int _sampleIndex;
    float _currentVoltage;
    float _averageVoltage;
    unsigned long _exposureStartTime;
    bool _isAlertActive;
    float readSensor();
    float calculateAverageVoltage();
    void triggerAlert();
};

#endif
