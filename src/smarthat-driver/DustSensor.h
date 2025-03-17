#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include <Arduino.h>


class DustSensor {
  private:
    int SENSOR_PIN;    
    int LED_PIN;       
    int ADC_MAX;      
    float VCC;         

  public:
   
    DustSensor(int sensorPin, int ledPin, int adcMax, float vcc);

    // Getters
    int getSensorPin() const;
    int getLedPin() const;
    int getAdcMax() const;
    float getVcc() const;

    // Setters
    void setSensorPin(int sensorPin);
    void setLedPin(int ledPin);
    void setAdcMax(int adcMax);
    void setVcc(float vcc);

    /**
     * Convert raw ADC value to voltage
     * @param rawADC: Raw ADC reading
     * @return: Voltage equivalent based on VCC and ADC_MAX
     */
    float getVoltage(int rawADC);
    
    
    float readDustSensor();
    
    
    void setUpDustSensor();
};

#endif // DUST_SENSOR_H 