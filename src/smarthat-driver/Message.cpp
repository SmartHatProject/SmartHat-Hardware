#include "Message.h"

const std::string Message::DUST_SENSOR_DATA = "DUST_SENSOR_DATA";
const std::string Message::SOUND_SENSOR_DATA = "SOUND_SENSOR_DATA";

Message::Message()
        : messageType("UNKNOWN"), data(0.0f), timeStamp(millis()) {}


Message::Message(std::string messageType, float data)
        : messageType(messageType), data(data), timeStamp(millis()) {}

std::string Message::getJsonMessage() {
    
    StaticJsonDocument<200> jsonMessage;
    
    // Clear any previous data
    jsonMessage.clear();
    
    // Add the fields to the JSON document
    jsonMessage["messageType"] = this->messageType;
    jsonMessage["data"] = this->data;
    jsonMessage["timeStamp"] = this->timeStamp;
    
    // Create a string to hold the JSON
    std::string jsonString;
   
    if (serializeJson(jsonMessage, jsonString) == 0) {
        Serial.println("Failed to serialize JSON!");
        return "{\"error\":\"serialization failed\"}";
    }
    
    // Debug output
    Serial.println("Generated JSON:");
    Serial.println(jsonString.c_str());
    
    // Verify the JSON is valid by trying to parse it back
    StaticJsonDocument<200> verifyDoc;
    DeserializationError error = deserializeJson(verifyDoc, jsonString);
    
    if (error) {
        Serial.print("JSON verification failed: ");
        Serial.println(error.c_str());
        return "{\"error\":\"invalid json\"}";
    }
    
    
    if (verifyDoc["data"] != this->data) {
        Serial.println("Data verification failed!");
        return "{\"error\":\"data mismatch\"}";
    }
    
    return jsonString;
}