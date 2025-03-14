#ifndef SMARTHAT_HARDWARE_MESSAGE_H
#define SMARTHAT_HARDWARE_MESSAGE_H

#include <string>
#include <ArduinoJson.h>

class Message {
    public:
       
        Message();
        Message(std::string messageType, float data);
        
     
        std::string getJsonMessage();
        
     
        static const std::string DUST_SENSOR_DATA;
        static const std::string SOUND_SENSOR_DATA;
       
        std::string getMessageType() const { return messageType; }
        float getData() const { return data; }
        unsigned long getTimeStamp() const { return timeStamp; }

    private:
        const std::string messageType;
        const float data;
        const unsigned long timeStamp;
};

#endif //SMARTHAT_HARDWARE_MESSAGE_H
