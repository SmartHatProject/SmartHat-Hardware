#ifndef SMARTHAT_HARDWARE_MESSAGE_H
#define SMARTHAT_HARDWARE_MESSAGE_H

#include <string>
#include <ArduinoJSON.h>

class Message {
    public:
        Message();
        Message(std::string messageType, float data);
        std:: string getJsonMessage();

        static const std::string DUST_DATA_MESSAGE;
        static const std::string SOUND_DATA_MESSAGE;

        
        // const std::string getMessageType() const { return messageType; }  //had to change this to std::string because we don't need char arrays anymore
        // float getData() const { return data; }
        // unsigned long getTimeStamp() const { return timeStamp; }

    private:
        const std::string messageType;
        const float data;
        const unsigned long timeStamp;

        //TODO: implement in .cpp file
        // std::string createFallbackJson();
};



#endif //SMARTHAT_HARDWARE_MESSAGE_H
