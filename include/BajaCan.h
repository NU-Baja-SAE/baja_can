#pragma once
#include "driver/twai.h"


typedef enum {
    FLOAT,
    INT32,
    UINT8,
    BOOL,
    BYTES
} CanDataType;


class CanMessage {
public:
    CanMessage(uint32_t id, const float value);
    CanMessage(uint32_t id, const int value);
    CanMessage(uint32_t id, const uint8_t* data, uint8_t len);
    const twai_message_t& getFrame() const;
    CanDataType getDataType() const { return dataType; }

    // typed getters
    float getFloat() const { return value.asFloat; }
    int32_t getInt32() const { return value.asInt32; }
    uint8_t getUInt8() const { return value.asUInt8; }
    bool getBool() const { return value.asBool; }
    // const uint8_t* getBytes() const { return value.asBytes; }
    
private:
    CanDataType dataType;
    twai_message_t frame;
    
    union {
        float asFloat;
        int32_t asInt32;
        UINT8_t asUInt8;
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

