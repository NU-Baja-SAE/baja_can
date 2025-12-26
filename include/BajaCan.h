#pragma once
#include "driver/twai.h"


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



class CanMessage {
public:
    CanMessage(uint32_t id, const float value);
    CanMessage(uint32_t id, const int value);
    CanMessage(uint32_t id, const uint8_t* data, uint8_t len);
    twai_message_t getFrame() const;
private:
    twai_message_t frame;
};    
