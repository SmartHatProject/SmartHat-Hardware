
#include "NoiseSensor.h"

NoiseSensor::NoiseSensor(int pin, float calibrationDb, int peakRef, int avgRef)
    : _pin(pin), _bias(2048), _loopTime(1000), _splRef(calibrationDb), 
      _peakRef(peakRef), _avgRef(avgRef), _k(0.15) {}

void NoiseSensor::begin() {
    analogReadResolution(12); // 12-bit ADC (0 - 4095)
}

void NoiseSensor::update() {
    _maxValue = 0;
    _sum = 0;
    _n = 0;
    _readStartTime = millis();

    while (millis() - _readStartTime < _loopTime) {
        int rawADC = readNoiseSensor();
        int adjustedADC = abs(rawADC - _bias);

        // Apply exponential weighting
        float weightedADC = adjustedADC * (1 + _k * exp((adjustedADC - 400) / 200.0));

        if (weightedADC > _maxValue)
            _maxValue = weightedADC;

        _sum += weightedADC;
        _n++;
    }

    _average = (float)_sum / _n;
    _dBSPLPeak = _splRef + 20 * log10((float)_maxValue / _peakRef);
    _dBSPLAvg = _splRef + 20 * log10((float)_average / _avgRef);
}

float NoiseSensor::getPeakDB() {
    return _dBSPLPeak;
}

float NoiseSensor::getAverageDB() {
    return _dBSPLAvg;
}

int NoiseSensor::readNoiseSensor() {
    return analogRead(_pin);
}
