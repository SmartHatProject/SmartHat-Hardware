#include "Message.h"

const std::string Message::DUST_DATA_MESSAGE = "DUST_SENSOR_DATA";
const std::string Message::SOUND_DATA_MESSAGE = "SOUND_SENSOR_DATA";

Message::Message()
        : messageType("UNKNOWN"), data(0.0f), timeStamp(millis()) {}


Message::Message(std::string messageType, float data)
        : messageType(messageType), data(data), timeStamp(millis()) {}

std::string Message::getJsonMessage() {
    StaticJsonDocument<100> jsonMessage;

    jsonMessage["messageType"] = this->messageType;
    jsonMessage["data"] = this->data;
    jsonMessage["timeStamp"]= this->timeStamp;

    std::string jsonString;
    serializeJson(jsonMessage, jsonString);

    // Debug output
    Serial.println( jsonString.c_str()); // Debug

    return jsonString;
    
    
}