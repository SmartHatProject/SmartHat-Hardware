#include "DustSensor.h"

// Constructor
DustSensor::DustSensor(int ledPin, int sensorPin, float dustThreshold)
    : _ledPin(ledPin), _sensorPin(sensorPin), _dustThreshold(dustThreshold),
      _voltage(0.0), _dustDensity(0.0) {}

// Initialize the sensor
void DustSensor::begin() {
    pinMode(_ledPin, OUTPUT);
    Serial.begin(115200);
}

// Read and process sensor data
void DustSensor::readSensor() {
    digitalWrite(_ledPin, HIGH);  // Turn LED ON
    delayMicroseconds(280);       // Wait for 0.28ms before sampling

    int sensorValue = analogRead(_sensorPin);  // Read sensor value

    delayMicroseconds(40);        // Additional time to complete 0.32ms ON time
    digitalWrite(_ledPin, LOW);   // Turn LED OFF

    delay(9.68);  // Ensure total cycle time is ~10ms

    // Convert to voltage
    _voltage = sensorValue * (3.3 / 4095.0);
    _dustDensity = voltageToDensity(_voltage);

    // Print results to Serial
    Serial.print("Voltage: ");
    Serial.print(_voltage);
    Serial.print(" V, Dust Density: ");
    Serial.print(_dustDensity);
    Serial.println(" mg/m³");

    // Check if dust density exceeds the threshold
    if (_dustDensity > _dustThreshold) {
        Serial.println("Alert: High dust density detected");
    }

    delay(250);  // Adjust as needed for your application
}

// Convert voltage to dust density
float DustSensor::voltageToDensity(float voltage) {
    // Define the voltage ranges and corresponding dust densities
    const float minVoltage1 = 0.6;   // Minimum voltage for the first segment
    const float maxVoltage1 = 3.5;   // Maximum voltage for the first segment
    const float minVoltage2 = 3.5;   // Minimum voltage for the second segment
    const float maxVoltage2 = 3.7;   // Maximum voltage for the second segment

    const float minDensity1 = 0.0;   // Minimum dust density for the first segment
    const float maxDensity1 = 0.5;   // Maximum dust density for the first segment
    const float minDensity2 = 0.5;   // Minimum dust density for the second segment
    const float maxDensity2 = 0.8;   // Maximum dust density for the second segment

    // Clamp the voltage to the expected range
    if (voltage < minVoltage1) voltage = minVoltage1;
    if (voltage > maxVoltage2) voltage = maxVoltage2;

    // Map the voltage to dust density based on the segment
    float dustDensity;
    if (voltage <= maxVoltage1) {
        // First segment: 0.6V to 3.5V
        dustDensity = map(voltage, minVoltage1, maxVoltage1, minDensity1, maxDensity1);
    } else {
        // Second segment: 3.5V to 3.7V
        dustDensity = map(voltage, minVoltage2, maxVoltage2, minDensity2, maxDensity2);
    }

    return dustDensity;
}

// Helper function to map a value from one range to another
float DustSensor::map(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

// Getters
float DustSensor::getVoltage() const {
    return _voltage;
}

float DustSensor::getDustDensity() const {
    return _dustDensity;
}

float DustSensor::getDustThreshold() const {
    return _dustThreshold;
}

// Setters
void DustSensor::setDustThreshold(float dustThreshold) {
    _dustThreshold = dustThreshold;
}