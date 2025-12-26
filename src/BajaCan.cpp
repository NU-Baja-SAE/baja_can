#include "BajaCan.h"
#include "string.h"

/**
 * @brief Construct a new Baja Can:: Baja Can object
 *
 * @param tx Transmit pin
 * @param rx Receive pin
 */
BajaCan::BajaCan(gpio_num_t tx, gpio_num_t rx)
    : initialized(false), txPin(tx), rxPin(rx)
{
}

/**
 * @brief Installs twai driver and starts it. Use default config.
 *
 * @return esp_err_t from twai_driver_install and twai_start
 */
esp_err_t BajaCan::begin()
{
    if (initialized)
    {
        return ESP_OK; // Already initialized
    }

    // Default configurations
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        txPin,
        rxPin,
        TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    esp_err_t ret = twai_driver_install(&g_config, &t_config, &f_config);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Start TWAI driver
    ret = twai_start();
    if (ret == ESP_OK)
    {
        initialized = true;
    }
    return ret;
}

/**
 * @brief Reads a CAN frame from the bus with timeout. This is a blocking call.
 *
 * @param frame Pointer to twai_message_t to store received frame
 * @param timeoutMs Timeout in milliseconds
 *
 * @return esp_err_t ESP_OK on success, ESP_ERR_TIMEOUT on timeout,
 *                    other esp_err_t from twai_receive
 */
esp_err_t BajaCan::readFrame(twai_message_t* frame, uint32_t timeoutMs)
{
    if (!initialized)
    {
        return ESP_ERR_INVALID_STATE; // Not initialized
    }   
    return twai_receive(frame, pdMS_TO_TICKS(timeoutMs));
}

/**
 * @brief Writes a CAN frame to the bus with timeout. This is a blocking call.
 *
 * @param frame Pointer to twai_message_t to send
 * @param timeoutMs Timeout in milliseconds
 *
 * @return esp_err_t ESP_OK on success, other esp_err_t from twai_transmit
 */
esp_err_t BajaCan::writeFrame(const twai_message_t* frame, uint32_t timeoutMs)
{
    if (!initialized)
    {
        return ESP_ERR_INVALID_STATE; // Not initialized
    }
    return twai_transmit(frame, pdMS_TO_TICKS(timeoutMs));
}   

/**
 * @brief send a CanMessage object
 * 
 * @param message Reference to CanMessage object to send
 * @param timeoutMs Timeout in milliseconds
 * @return esp_err_t ESP_OK on success, other esp_err_t from twai_transmit
 */
esp_err_t BajaCan::writeMessage(const CanMessage& message, uint32_t timeoutMs)
{
    return writeFrame(&message.getFrame(), timeoutMs);
}

/**
 * @brief read a CanMessage object
 * 
 * @param message Reference to CanMessage object to fill
 * @param timeoutMs Timeout in milliseconds
 * @return esp_err_t ESP_OK on success, ESP_ERR_TIMEOUT on timeout,
 *                    other esp_err_t from twai_receive
 */
esp_err_t BajaCan::readMessage(CanMessage& message, uint32_t timeoutMs)
{
    twai_message_t frame;
    esp_err_t ret = readFrame(&frame, timeoutMs);
    if (ret == ESP_OK)
    {
        // Copy received frame into CanMessage object
        message = CanMessage(frame.identifier, frame.data, frame.data_length_code);
    }
    return ret;
}



// CanMessage class implementation

/**
 * @brief Construct a new Can Message:: Can Message object with float data
 * 
 * @param id ID of the CAN message
 * @param value Float value to send
 */
CanMessage::CanMessage(uint32_t id, const float value)
{
    frame.identifier = id;
    frame.data_length_code = sizeof(float);
    memcpy(frame.data, &value, sizeof(float));
}

/**
 * @brief Construct a new Can Message:: Can Message object with int data
 * 
 * @param id ID of the CAN message
 * @param value Int value to send
 */
CanMessage::CanMessage(uint32_t id, const int value)
{
    frame.identifier = id;
    frame.data_length_code = sizeof(int);
    memcpy(frame.data, &value, sizeof(int));
}

/**
 * @brief Construct a new Can Message:: Can Message object with raw data
 * 
 * @param id ID of the CAN message
 * @param data Pointer to data bytes
 * @param len Length of data in bytes
 */
CanMessage::CanMessage(uint32_t id, const uint8_t* data, uint8_t len)
{
    frame.identifier = id;
    frame.data_length_code = len;
    memcpy(frame.data, data, len);
}

// Getter for the underlying twai_message_t frame
twai_message_t CanMessage::getFrame() const
{
    return frame;
}

