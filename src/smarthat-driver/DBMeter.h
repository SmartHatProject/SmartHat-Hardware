#ifndef DBMETER_H
#define DBMETER_H

#include <Arduino.h>

class DBMeter {
public:
    DBMeter(int pin, float calibrationDb, int peakRef, int avgRef);
    void begin();
    void update();
    float getPeakDB();
    float getAverageDB();

private:
    int _pin;
    int _bias;
    int _maxValue;
    float _loopTime;
    int _splRef;
    int _peakRef;
    int _avgRef;
    float _k;
    
    unsigned long _sum;
    int _n;
    unsigned long _readStartTime;
    float _average;
    float _dBSPLPeak;
    float _dBSPLAvg;

    int readSensor();
};

#endif
