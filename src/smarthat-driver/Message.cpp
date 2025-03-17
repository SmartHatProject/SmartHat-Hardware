#include "Message.h"


// Message types that match the Android app
const char* Message::DUST_SENSOR_DATA = "DUST_SENSOR_DATA";
const char* Message::SOUND_SENSOR_DATA = "SOUND_SENSOR_DATA";

Message::Message()
        : messageType("UNKNOWN"), data(0.0f), timeStamp(millis()) {}


Message::Message(const char* messageType, float data)
        : messageType(messageType), data(data), timeStamp(millis()) {}
//  from std::string to string 
// also removedg verification steps

String Message::getJsonMessage() {
    // Use fixed size document to avoid heap fragmentation
    StaticJsonDocument<200> jsonDoc;
    
    // Clear any previous content
    jsonDoc.clear();
    
    // Add data using error checking
    bool jsonCreationSuccess = true;
    
    // Check for valid messageType
    if (messageType == nullptr || strlen(messageType) == 0) {
        Serial.println("ERROR: Invalid messageType in Message");
        jsonCreationSuccess = false;
    } else if (strcmp(messageType, DUST_SENSOR_DATA) != 0 && 
               strcmp(messageType, SOUND_SENSOR_DATA) != 0) {
        // Verify messageType matches one of the expected constants
        Serial.print("ERROR: Unknown messageType: ");
        Serial.println(messageType);
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
        if (strcmp(messageType, DUST_SENSOR_DATA) == 0) {
            // Dust sensors typically range from 0 to 1000 μg/m³
            if (data < 0.0f || data > 1000.0f) {
                Serial.print("WARNING: Dust value out of typical range: ");
                Serial.println(data);
                // Still include the value but log a warning
            }
        } else if (strcmp(messageType, SOUND_SENSOR_DATA) == 0) {
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
    String jsonString = "";
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
        if (jsonString.indexOf("messageType") == -1 ||
            jsonString.indexOf("data") == -1 ||
            jsonString.indexOf("timeStamp") == -1) {
            Serial.println("ERROR: JSON missing required fields");
            Serial.println(jsonString);
            return createFallbackJson();
        }
        
        // Log the successful JSON creation
        Serial.println("JSON created successfully:");
        Serial.print("  Message Type: ");
        Serial.println(this->messageType);
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

// New helper method to create fallback JSON with default values
String Message::createFallbackJson() {
    StaticJsonDocument<200> fallbackDoc;
    
    // Use the message type if valid, or a default if not
    const char* msgType = (messageType != nullptr && 
                          (strcmp(messageType, DUST_SENSOR_DATA) == 0 || 
                           strcmp(messageType, SOUND_SENSOR_DATA) == 0)) 
                         ? messageType 
                         : SOUND_SENSOR_DATA;
    
    fallbackDoc["messageType"] = msgType;
    
    // Use appropriate default values based on message type
    float defaultValue = 0.0f;
    if (strcmp(msgType, DUST_SENSOR_DATA) == 0) {
        defaultValue = 10.0f; 
    } else {
        defaultValue = 40.0f; 
    }
    
    fallbackDoc["data"] = defaultValue;
    fallbackDoc["timeStamp"] = millis();
    
    String fallbackJson = "";
    serializeJson(fallbackDoc, fallbackJson);
    
    Serial.println("Using fallback JSON due to validation failure: " + fallbackJson);
    return fallbackJson;
}
