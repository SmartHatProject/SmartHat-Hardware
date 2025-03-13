#define SENSOR_PIN 35  // Analog input pin (GPIO35)
#define LED_PIN 23     // LED control pin
#define ADC_MAX 4095   // 12-bit ADC resolution
#define VCC 3.3        // ESP32 ADC reference voltage

//works
float getVoltage(int rawADC) {
    return (VCC / ADC_MAX) * rawADC;  // Convert ADC to voltage
}

float readDensity() {
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(280);
    
    int adc = analogRead(SENSOR_PIN);
    float v0 = getVoltage(adc);
    
    // Adjusted formula for 3.3V supply
    float density = ((v0 - 0.6) / 0.5) * 100;  // ug/m³

    digitalWrite(LED_PIN, HIGH);
    
    return max(density, 0.0f);  // Prevent negative values
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    delay(1000);
}

void loop() {
    Serial.print("Dust: ");
    Serial.print(readDensity());
    Serial.println(" ug/m³");
    delay(1000);
}
