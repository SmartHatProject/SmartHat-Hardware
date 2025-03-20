#include <Arduino.h>
#include "DBMeter.h"

#define AUDIO_PIN 34

DBMeter meter(AUDIO_PIN, 57.0, 373, 364);

void setup() {
    Serial.begin(115200);
    meter.begin();
}

void loop() {
    meter.update();
    
    Serial.print("Peak SPL: ");
    Serial.print(meter.getPeakDB(), 1);
    Serial.print(" dB    ");

    Serial.print("Average SPL: ");
    Serial.print(meter.getAverageDB(), 1);
    Serial.println(" dB");

    delay(500); // Update every 500ms
}
