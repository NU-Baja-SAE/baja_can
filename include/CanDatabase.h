#pragma once
#include "BajaCan.h"
#include <stddef.h>

typedef struct
{
    uint32_t id;
    CanDataType type;
    uint8_t dlc;
} CanMessageDef;

namespace CanDatabase
{

    // 1xx data for HUD
    // 2xx data for debugging and tuning, not sent to HUD
    // 3xx arbitrary messages for testing

    constexpr CanMessageDef ENGINE_RPM = {0x100, FLOAT, sizeof(float)};
    constexpr CanMessageDef SECONDARY_RPM = {0x101, FLOAT, sizeof(float)};

    constexpr CanMessageDef MOTOR_SETPOINT = {0x200, INT32, sizeof(int32_t)};

    constexpr CanMessageDef INT_DATA = {0x300, INT32, sizeof(int32_t)};
    constexpr CanMessageDef INT8_DATA = {0x301, UINT8, 1};

    constexpr CanMessageDef canDatabase[] = {
        ENGINE_RPM,
        SECONDARY_RPM,
        MOTOR_SETPOINT,
        INT_DATA,
        INT8_DATA};

    constexpr size_t canDatabaseSize = sizeof(canDatabase) / sizeof(canDatabase[0]);

}