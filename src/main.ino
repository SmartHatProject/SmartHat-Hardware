//this is an example code , + loop to send sensor data as Json
#include <BluetoothSerial.h>
#include <ArduinoJSON.h>

BluetoothSerial SerialBT;
void setup() {
    Serial.begin(115200);
    SerialBT.begin("SmartHat");
}

void loop() {
    // Send dust sensor data as JSON
    StaticJsonDocument<200> doc;
    doc["sensor"] = "dust";
    doc["value"] = analogRead(A0);
    doc["timestamp"] = millis();

    serializeJson(doc, SerialBT);
    SerialBT.println();
    delay(1000);
}