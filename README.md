# Baja Can
Can Library for NU Baja


## Usage

You should include this library as a submodule in your git repository. Run this command in your Platform IO project
```
git submodule add https://github.com/NU-Baja-SAE/baja_can.git lib/baja_can
```

This will copy the CAN library into lib directory, and it will be accessible to your project.

## API Reference

### Types

`CanDataType`:
- `FLOAT`
- `INT32`
- `UINT8`
- `BOOL`
- `BYTES`
- `NONE`

`CanMessageDef` (in `/include/CanDatabase.h`):
- `id`: CAN message identifier
- `type`: expected `CanDataType`
- `dlc`: minimum payload length (bytes) required for that ID

### `CanMessage`

Constructors:
- `CanMessage(uint32_t id, float value)` -> builds a 4-byte float payload
- `CanMessage(uint32_t id, int32_t value)` -> builds a 4-byte signed integer payload
- `CanMessage(uint32_t id, const uint8_t* data, uint8_t len)` -> raw payload (`len` is clamped to 8)
- `CanMessage()` -> empty/default message

Accessors:
- `const twai_message_t& getFrame() const`
- `CanDataType getDataType() const`
- `uint32_t getId() const`
- `float getFloat() const`
- `int32_t getInt32() const`
- `uint8_t getUInt8() const`
- `bool getBool() const`
- `const uint8_t* getBytes() const`
- `uint8_t getDataLength() const`

Notes:
- `CanMessage` uses `twai_message_t` as the single source of truth.
- Typed getters decode directly from frame bytes.
- Check `getDataType()` before calling typed getters.

### `BajaCan`

Constructor:
- `BajaCan(gpio_num_t tx, gpio_num_t rx)`

Lifecycle:
- `esp_err_t begin()` -> installs and starts TWAI driver
- `esp_err_t end()` -> stops and uninstalls TWAI driver

Raw frame I/O:
- `esp_err_t readFrame(twai_message_t* frame, uint32_t timeoutMs)`
- `esp_err_t writeFrame(const twai_message_t* frame, uint32_t timeoutMs)`

Message I/O:
- `esp_err_t writeMessage(const CanMessage& message, uint32_t timeoutMs)`
- `esp_err_t readMessage(CanMessage& message, uint32_t timeoutMs)`

`readMessage` behavior:
- Reads one frame and stores it in `CanMessage`.
- Uses `canDatabase` (`/include/CanDatabase.h`) to assign `CanDataType` by message ID.
- Defaults to `BYTES` when ID is not in the database.
- Returns `ESP_ERR_INVALID_RESPONSE` if received DLC is shorter than the database definition for that ID.

Common return values:
- `ESP_OK`
- `ESP_ERR_TIMEOUT`
- `ESP_ERR_INVALID_STATE`
- `ESP_ERR_INVALID_RESPONSE`

### Quick Example

```cpp
#include "BajaCan.h"

BajaCan can(GPIO_NUM_5, GPIO_NUM_4);

void setup() {
    can.begin();
}

void loop() {
    CanMessage msg;
    if (can.readMessage(msg, 50) == ESP_OK) {
        if (msg.getDataType() == FLOAT) {
            float rpm = msg.getFloat();
            (void)rpm;
        } else if (msg.getDataType() == INT32) {
            int32_t value = msg.getInt32();
            (void)value;
        } else {
            const uint8_t* bytes = msg.getBytes();
            uint8_t len = msg.getDataLength();
            (void)bytes;
            (void)len;
        }
    }
}
```
