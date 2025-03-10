#ifndef DUSTSENSOR_H
#define DUSTSENSOR_H

#include <Arduino.h>

class DustSensor {
public:
    // Constructor
    DustSensor(int ledPin, int sensorPin, float dustThreshold);

    // Initialize the sensor
    void begin();

    // Read and process sensor data
    void readSensor();

    // Getters
    float getVoltage() const;
    float getDustDensity() const;
    float getDustThreshold() const;

    // Setters
    void setDustThreshold(float dustThreshold);

private:
    // Pin definitions
    int _ledPin;
    int _sensorPin;

    // Dust density threshold
    float _dustThreshold;

    // Sensor data
    float _voltage;
    float _dustDensity;

    // Convert voltage to dust density
    float voltageToDensity(float voltage);

    // Helper function to map values
    float map(float value, float fromLow, float fromHigh, float toLow, float toHigh);
};

#endif