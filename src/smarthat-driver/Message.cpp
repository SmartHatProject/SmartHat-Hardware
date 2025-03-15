#include "Message.h"

const char* Message::DUST_SENSOR_DATA = "DUST_SENSOR_DATA";
const char* Message::SOUND_SENSOR_DATA = "SOUND_SENSOR_DATA";

Message::Message()
        : messageType("UNKNOWN"), data(0.0f), timeStamp(millis()) {}


Message::Message(const char* messageType, float data)
        : messageType(messageType), data(data), timeStamp(millis()) {}
//  from std::string to string 
// also removedg verification steps

String Message::getJsonMessage() {
    
    StaticJsonDocument<200> jsonMessage;
    

    jsonMessage.clear();
    jsonMessage["messageType"] = this->messageType;
    jsonMessage["data"] = this->data;
    jsonMessage["timeStamp"] = this->timeStamp;
    
   
    String jsonString;
   
    serializeJson(jsonMessage, jsonString);
    
    // Enhanced debug output
    Serial.println("Generated JSON:");
    Serial.print("  Message Type: ");
    Serial.println(this->messageType);
    Serial.print("  Data Value: ");
    Serial.println(this->data);
    Serial.print("  Timestamp: ");
    Serial.println(this->timeStamp);
    Serial.println(jsonString);
    
    
    
    return jsonString;
}
