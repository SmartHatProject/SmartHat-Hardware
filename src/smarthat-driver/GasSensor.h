#ifndef GASSENSOR_H
#define GASSENSOR_H

class MQ135Sensor {
public:
    MQ135Sensor(int pin, float loadResistance, int numSamples);
    void begin();
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
