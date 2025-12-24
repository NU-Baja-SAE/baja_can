#pragma once
#include "driver/twai.h"


class BajaCan {
public:
    BajaCan(gpio_num_t tx, gpio_num_t rx);
    esp_err_t begin();
    esp_err_t readFrame(twai_message_t* frame, uint32_t timeoutMs);
    esp_err_t writeFrame(const twai_message_t* frame, uint32_t timeoutMs);

private:
    bool initialized;
    gpio_num_t txPin;
    gpio_num_t rxPin;

};