#ifndef GASSENSOR_H
#define GASSENSOR_H

#include <Arduino.h>

class GasSensor {
public:
    GasSensor(int pin, float loadResistance, int numSamples);
    void begin();
    void update();
    float calibrateR0();
    float readPPM();

private:
    int _pin;
    float _rl;
    int _numSamples;
    float _r0;

    float getAverageAnalog();
    float calculateRs(float analogValue);
    float getPPM(float rs, float r0);
};

#endif
