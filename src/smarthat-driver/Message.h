#ifndef SMARTHAT_HARDWARE_MESSAGE_H
#define SMARTHAT_HARDWARE_MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
// changed std::string to const char

class Message {
    public:
       
        Message();
        Message(const char* messageType, float data);
        
     
        String getJsonMessage();
        
     
        static const char* DUST_SENSOR_DATA;
        static const char* SOUND_SENSOR_DATA;
       
        const char* getMessageType() const { return messageType; }
        float getData() const { return data; }
        unsigned long getTimeStamp() const { return timeStamp; }

    private:
        const char* messageType;
        const float data;
        const unsigned long timeStamp;
};

#endif //SMARTHAT_HARDWARE_MESSAGE_H
