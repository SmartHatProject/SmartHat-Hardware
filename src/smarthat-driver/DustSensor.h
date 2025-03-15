#include <Arduino.h>

class DustSensor{

  private:
    int SENSOR_PIN;
    int LED_PIN;
    int ADC_MAX;
    float VCC;

    public:
      DustSensor(int,int,int,float);

      // Getters
      int getSensorPin() const;
      int getLedPin() const;
      int getAdcMax() const;
      float getVcc() const;

      // Setters
      void setSensorPin(int);
      void setLedPin(int);
      void setAdcMax(int);
      void setVcc(float);

      float getVoltage(int);
      float readDustSensor();
      void setUpDustSensor();
      
      


};