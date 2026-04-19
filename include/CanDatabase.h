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

    // All CAN IDs follow this scheme, first three bits in binary represent sender (100) = ECVT, (010) = DAQ, (001) = HUD. Next three bits follow the same scheme but for receiver (more than one receiver possible) and then last 5 bits are message ids (32 possibilities)

    constexpr CanMessageDef ENGINE_RPM = {0x441, FLOAT, sizeof(float)};
    constexpr CanMessageDef SECONDARY_RPM = {0x442, FLOAT, sizeof(float)};
    constexpr CanMessageDef MODE = {0x443, UINT8, sizeof(uint8_t)};
    constexpr CanMessageDef STATUS = {0x444, UINT8, sizeof(uint8_t)};
    constexpr CanMessageDef ECVT_TEMP = {0x445, FLOAT, sizeof(float)};
    constexpr CanMessageDef MOTOR_SETPOINT = {0x446, INT32, sizeof(int32_t)};
    
    constexpr CanMessageDef FUEL_LEVEL = {0x222, FLOAT, sizeof(float)};
    constexpr CanMessageDef LINEAR_SPEED = {0x221, FLOAT, sizeof(float)};
    
    constexpr CanMessageDef BRAKE_POT = {0x141, FLOAT, sizeof(float)};
    constexpr CanMessageDef FRONT_LEFT_WHEEL_SPEED = {0x142, FLOAT, sizeof(float)};
    constexpr CanMessageDef FRONT_RIGHT_WHEEL_SPEED = {0x143, FLOAT, sizeof(float)};




    

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