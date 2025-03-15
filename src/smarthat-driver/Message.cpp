#include "Message.h"

const char* Message::DUST_SENSOR_DATA = "DUST_SENSOR_DATA";
const char* Message::SOUND_SENSOR_DATA = "SOUND_SENSOR_DATA";

Message::Message()
        : messageType("UNKNOWN"), data(0.0f), timeStamp(millis()) {}


Message::Message(const char* messageType, float data)
        : messageType(messageType), data(data), timeStamp(millis()) {}

String Message::getJsonMessage() {
    
    StaticJsonDocument<200> jsonMessage;
    

    jsonMessage.clear();
    jsonMessage["messageType"] = this->messageType;
    jsonMessage["data"] = this->data;
    jsonMessage["timeStamp"] = this->timeStamp;
    
   
    String jsonString;
   
    serializeJson(jsonMessage, jsonString);
    
    // debug output
    Serial.println("Generated JSON:");
    Serial.println(jsonString);
    
    return jsonString;
}
