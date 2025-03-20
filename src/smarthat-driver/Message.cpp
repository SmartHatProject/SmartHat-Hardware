#include "Message.h"

const std::string Message::DUST_DATA_MESSAGE = "DUST_SENSOR_DATA";
const std::string Message::SOUND_DATA_MESSAGE = "SOUND_SENSOR_DATA";

Message::Message()
        : messageType("UNKNOWN"), data(0.0f), timeStamp(millis()) {}


Message::Message(std::string messageType, float data)
        : messageType(messageType), data(data), timeStamp(millis()) {}




std:: string Message::getJsonMessage() {
    // Use fixed size document to avoid heap fragmentation
    StaticJsonDocument<200> jsonDoc;
    
    // Clear any previous content
    jsonDoc.clear();
    
    // Add data using error checking
    bool jsonCreationSuccess = true;
    
    // Check for valid messageType
    if (messageType.empty()) {
        Serial.println("ERROR: Invalid messageType in Message");
        jsonCreationSuccess = false;
    } else if (messageType != DUST_DATA_MESSAGE && messageType != SOUND_DATA_MESSAGE) {
        // Verify messageType matches one of the expected constants
        Serial.print("ERROR: Unknown messageType: ");
        Serial.println(messageType.c_str());
        Serial.println("Expected DUST_SENSOR_DATA or SOUND_SENSOR_DATA");
        jsonCreationSuccess = false;
    } else {
        jsonDoc["messageType"] = this->messageType;
    }
    
    // Check data is within reasonable range
    if (isnan(data) || isinf(data)) {
        Serial.println("ERROR: Invalid sensor data value (NaN or Inf)");
        jsonCreationSuccess = false;
    } else {
        // Additional range checks for specific message types
        if (messageType == DUST_DATA_MESSAGE) {
            // Dust sensors typically range from 0 to 1000 μg/m³
            if (data < 0.0f || data > 1000.0f) {
                Serial.print("WARNING: Dust value out of typical range: ");
                Serial.println(data);
                // Still include the value but log a warning
            }
        } else if (messageType == SOUND_DATA_MESSAGE) {
            // Sound levels typically range from 30 to 130 dB
            if (data < 30.0f || data > 130.0f) {
                Serial.print("WARNING: Sound value out of typical range: ");
                Serial.println(data);
                // Still include the value but log a warning
            }
        }
        
        jsonDoc["data"] = this->data;
    }
    
    // Add timestamp
    unsigned long currentTimestamp = this->timeStamp;
    if (currentTimestamp == 0) {
        // If timestamp wasn't set properly, use current time
        currentTimestamp = millis();
        Serial.println("WARNING: Using current time as timestamp was zero");
    }
    jsonDoc["timeStamp"] = currentTimestamp;
    
    // Only proceed if JSON creation was successful
    std::string jsonString = "";
    if (jsonCreationSuccess) {
        // Validate document size before serializing
        size_t jsonSize = measureJson(jsonDoc);
        if (jsonSize > 200) {
            Serial.print("ERROR: JSON document too large: ");
            Serial.println(jsonSize);
            return createFallbackJson();
        }
        
        // Serialize with error checking
        serializeJson(jsonDoc, jsonString);
        
        // Validation check
        if (jsonString.length() == 0 || jsonString == "null" || jsonString == "{}") {
            Serial.println("ERROR: JSON serialization failed");
            return createFallbackJson();
        }
        
        // Verify JSON format matches expected structure
        // Verify JSON format matches expected structure
        if (jsonString.find("messageType") == std::string::npos ||
            jsonString.find("data") == std::string::npos ||
            jsonString.find("timeStamp") == std::string::npos) {
            Serial.println("ERROR: JSON missing required fields");
            Serial.println(jsonString.c_str());
            return createFallbackJson();
        }
        
        // Log the successful JSON creation
        Serial.println("JSON created successfully:");
        Serial.print("  Message Type: ");
        Serial.println(this->messageType.c_str());
        Serial.print("  Data Value: ");
        Serial.println(this->data);
        Serial.print("  Timestamp: ");
        Serial.println(currentTimestamp);
        Serial.print("  JSON Size: ");
        Serial.print(jsonSize);
        Serial.print(" bytes, String Length: ");
        Serial.println(jsonString.length());
    } else {
        Serial.println("ERROR: Failed to create JSON message due to validation errors");
        return createFallbackJson();
    }
    
    return jsonString;
}



std::string Message::createFallbackJson() {
    StaticJsonDocument<200> fallbackDoc;
    
    // Use the message type if valid, or a default if not
    const std::string msgType = (messageType == DUST_DATA_MESSAGE || messageType == SOUND_DATA_MESSAGE) 
                         ? messageType 
                         : SOUND_DATA_MESSAGE;
    
    fallbackDoc["messageType"] = msgType;
    
    // Use appropriate default values based on message type
    float defaultValue = 0.0f;
    if (msgType == SOUND_DATA_MESSAGE) {
        defaultValue = 10.0f; 
    } else {
        defaultValue = 40.0f; 
    }
    
    fallbackDoc["data"] = defaultValue;
    fallbackDoc["timeStamp"] = millis();
    
    std::string fallbackJson = "";
    serializeJson(fallbackDoc, fallbackJson);
    
    Serial.println(("Using fallback JSON due to validation failure: " + fallbackJson).c_str());
    return fallbackJson;
}