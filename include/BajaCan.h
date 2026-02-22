#pragma once

#ifndef BAJA_CAN_H
#define BAJA_CAN_H

#include "driver/twai.h"
#include <stdint.h>

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
    CanMessage(uint32_t id, const int32_t value);
    CanMessage(uint32_t id, const uint8_t* data, uint8_t len);
    CanMessage();
    const twai_message_t& getFrame() const;
    CanDataType getDataType() const { return dataType; }

    uint32_t getId() const { return frame.identifier; }

    // Typed getters decode from frame bytes. Callers should check getDataType() first.
    float getFloat() const;
    int32_t getInt32() const;
    uint8_t getUInt8() const;
    bool getBool() const;
    const uint8_t* getBytes() const { return frame.data; }
    uint8_t getDataLength() const { return frame.data_length_code; }

    // setters
    void setDataType(CanDataType type) { dataType = type; }
    void setFrame(const twai_message_t& frame) { this->frame = frame; }

private:
    CanDataType dataType;
    twai_message_t frame;
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
