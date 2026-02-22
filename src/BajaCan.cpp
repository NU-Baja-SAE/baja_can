#include "BajaCan.h"
#include "CanDatabase.h"
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
 * @param timeoutMs Timeout in milliseconds, set to zero for non blocking, or portMAX_DELAY for blocking indefinitely
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
        // Always set the frame and start with a safe default type.
        message.setFrame(frame);
        message.setDataType(BYTES);

        for (size_t i = 0; i < canDatabaseSize; ++i)
        {
            const CanMessageDef& def = canDatabase[i];
            if (def.id == frame.identifier) {
                // Reject malformed frames that do not contain enough bytes for the declared type.
                if (frame.data_length_code < def.dlc)
                {
                    return ESP_ERR_INVALID_RESPONSE;
                }
                message.setDataType(def.type);
                break;
            }
        }
    }
    return ret;
}

/**
 * @brief Stops and uninstalls the TWAI driver.
 *
 * @return esp_err_t from twai_stop and twai_driver_uninstall
 */
esp_err_t BajaCan::end()
{
    if (!initialized)
    {  
        return ESP_OK; // Not initialized
    }
    esp_err_t ret = twai_stop();
    if (ret != ESP_OK)
    {
        return ret;
    }
    ret = twai_driver_uninstall();
    if (ret == ESP_OK)
    {
        initialized = false;
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
    memset(&frame, 0, sizeof(frame));
    dataType = FLOAT;
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
CanMessage::CanMessage(uint32_t id, const int32_t value)
{
    memset(&frame, 0, sizeof(frame));
    dataType = INT32;
    frame.identifier = id;
    frame.data_length_code = sizeof(int32_t);
    memcpy(frame.data, &value, sizeof(int32_t));
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
    memset(&frame, 0, sizeof(frame));
    dataType = BYTES;

    frame.identifier = id;

    // TWAI payload is capped at 8 bytes.
    const uint8_t clampedLen = (len > sizeof(frame.data)) ? sizeof(frame.data) : len;
    frame.data_length_code = clampedLen;
    if ((data != nullptr) && (clampedLen > 0U))
    {
        memcpy(frame.data, data, clampedLen);
    }
}

CanMessage::CanMessage() : dataType(NONE)
{
    memset(&frame, 0, sizeof(frame));
}

// Getter for the underlying twai_message_t frame
const twai_message_t& CanMessage::getFrame() const
{
    return frame;
}

float CanMessage::getFloat() const
{
    float out = 0.0F;
    if (frame.data_length_code >= sizeof(float))
    {
        memcpy(&out, frame.data, sizeof(float));
    }
    return out;
}

int32_t CanMessage::getInt32() const
{
    int32_t out = 0;
    if (frame.data_length_code >= sizeof(int32_t))
    {
        memcpy(&out, frame.data, sizeof(int32_t));
    }
    return out;
}

uint8_t CanMessage::getUInt8() const
{
    if (frame.data_length_code >= 1U)
    {
        return frame.data[0];
    }
    return 0U;
}

bool CanMessage::getBool() const
{
    return getUInt8() != 0U;
}

