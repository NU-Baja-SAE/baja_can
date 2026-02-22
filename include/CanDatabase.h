#pragma once
#include "BajaCan.h"
#include <stddef.h>

typedef struct {
    uint32_t id;
    CanDataType type;
    uint8_t dlc;
} CanMessageDef; 


constexpr CanMessageDef ENGINE_RPM = {0x100, FLOAT, 4};
constexpr CanMessageDef INT_DATA = {0x101, INT32, 4};
constexpr CanMessageDef INT8_DATA = {0x102, UINT8, 1};


constexpr CanMessageDef canDatabase[] = {
    ENGINE_RPM,
    INT_DATA,
    INT8_DATA
};

constexpr size_t canDatabaseSize = sizeof(canDatabase) / sizeof(canDatabase[0]);
