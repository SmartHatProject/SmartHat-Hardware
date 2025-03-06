#include "Message.h"

const std::string Message::DUST_DATA_MESSAGE = "DUST_SENSOR_DATA";
const std::string Message::SOUND_DATA_MESSAGE = "SOUND_SENSOR_DATA";

Message::Message()
        : messageType("UNKNOWN"), data(0.0f), timeStamp(millis()) {}


Message::Message(std::string messageType, float data)
        : messageType(messageType), data(data), timeStamp(millis()) {}


JsonDocument Message::getJsonMessage() {
    StaticJsonDocument<100> jsonMessage;

    jsonMessage["messageType"] = this->messageType;
    jsonMessage["data"] = this->data;
    jsonMessage["timeStamp"]= this->timeStamp;

    return jsonMessage;
}