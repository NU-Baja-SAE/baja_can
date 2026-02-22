#pragma once
#include "BajaCan.h"
#include <vector>

typedef struct {
    uint32_t id;
    CanDataType type;
} CanMessageDef; 


const CanMessageDef ENGINE_RPM = {0x100, FLOAT};
const CanMessageDef INT_DATA = {0x101, INT32};


const std::vector<CanMessageDef> canDatabase = {
    ENGINE_RPM,
    INT_DATA
};