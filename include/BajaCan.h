#pragma once

#ifndef BAJA_CAN_H
#define BAJA_CAN_H

#include "driver/twai.h"
#include "string.h"

typedef enum {
    FLOAT,
    INT32,
    UINT8,
    BOOL,
    BYTES,
    NONE
} CanDataType;


class CanMessage {
public:
    CanMessage(uint32_t id, const float value);
    CanMessage(uint32_t id, const int value);
    CanMessage(uint32_t id, const uint8_t* data, uint8_t len);
    CanMessage();
    const twai_message_t& getFrame() const;
    CanDataType getDataType() const { return dataType; }

    uint32_t getId() const { return frame.identifier; }

    // typed getters
    float getFloat() const { return value.asFloat; }
    int32_t getInt32() const { return value.asInt32; }
    uint8_t getUInt8() const { return value.asUInt8; }
    bool getBool() const { return value.asBool; }
    // const uint8_t* getBytes() const { return value.asBytes; }

    // setters
    void setDataType(CanDataType type) { dataType = type; }
    void setFrame(const twai_message_t& frame) { this->frame = frame; }
    
    void setValue(const uint8_t* data, uint8_t len) {
        // Store raw data in union for typed getters
        memset(&value, 0, sizeof(value)); // Clear previous value
        if (len > sizeof(value.asBytes)) {
            len = sizeof(value.asBytes); // Limit to max size of union
        }
        memcpy(value.asBytes, data, len);
    }
    
private:
    CanDataType dataType;
    twai_message_t frame;
    
    union {
        float asFloat;
        int32_t asInt32;
        uint8_t asUInt8;
        bool asBool;
        uint8_t asBytes[8];
    } value;
};


class BajaCan {
public:
    BajaCan(gpio_num_t tx, gpio_num_t rx);
    esp_err_t begin();
    esp_err_t readFrame(twai_message_t* frame, uint32_t timeoutMs);
    esp_err_t writeFrame(const twai_message_t* frame, uint32_t timeoutMs);
    esp_err_t writeMessage(const CanMessage& message, uint32_t timeoutMs);
    esp_err_t readMessage(CanMessage& message, uint32_t timeoutMs);
    esp_err_t end();

private:
    bool initialized;
    gpio_num_t txPin;
    gpio_num_t rxPin;

};

#endif // BAJA_CAN_H