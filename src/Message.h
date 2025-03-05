#ifndef SMARTHAT_HARDWARE_MESSAGE_H
#define SMARTHAT_HARDWARE_MESSAGE_H

#include <string>
#include <ArduinoJSON.h>

class Message {
    public:
        Message();
        Message(std::string messageType, float data);
        JsonDocument getJsonMessage();

        static const std::string DUST_DATA_MESSAGE;
        static const std::string SOUND_DATA_MESSAGE;

    private:
        const std::string messageType;
        const float data;
        const unsigned long timeStamp;
};



#endif //SMARTHAT_HARDWARE_MESSAGE_H
