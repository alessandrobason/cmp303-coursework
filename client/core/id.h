#pragma once

#include <utils/types.h>

struct UniqueId {
    u32 id = 0;
    UniqueId() = default;
    UniqueId(u32 id) : id(id) {}
    bool operator==(const UniqueId &other) { return id == other.id; }
};

struct TypeId {
    u32 id = 0;
    TypeId() = default;
    TypeId(u32 id) : id(id) {}
    bool operator==(const TypeId &other) { return id == other.id; }
};

extern u32 type_count;

template<typename T>
TypeId getUniqueTypeId(T *ptr = nullptr) {
    static u32 type_id = ++type_count;
    return type_id;
}

inline UniqueId getUniqueId() {
    static u32 id = 0;
    return ++id;
}
