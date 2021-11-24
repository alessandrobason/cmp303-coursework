#pragma once

#include "types.h"

extern u32 current_id;

template<typename T>
u32 GetUniqueId() {
    static u32 id = ++current_id;
    return id;    
}