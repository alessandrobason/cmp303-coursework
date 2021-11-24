#pragma once

#include <utils/types.h>
#include <utils/vec.h>

enum : u8 {
    MSG_ERR, MSG_READY,
    MSG_NEW_PLAYER, MSG_PLAYER_NAME,
    MSG_DIR, MSG_POS, MSG_ANIM, MSG_STATE,
    MSG_BOMB_REQ,
    MSG_OBJ_ADD, MSG_OBJ_REM,
    MSG_POW_ADD,
};

#pragma pack(push, 1)

template<typename T>
struct Message {
    u8 type = MSG_ERR;
    T data;
};

struct AddRemMsg {
    u8 id = 0;
    vec2i pos;
    AddRemMsg() = default;
    AddRemMsg(const u8 &id, const vec2i &pos)
        : id(id), pos(pos) {}
};

struct AddPowerup {
    u8 type;
    vec2i pos;
    AddPowerup() = default;
    AddPowerup(const u8 &type, const vec2i & pos)
        : type(type), pos(pos) {}
};

struct PlayerNew {
    u8 id;
    bool is_main;
};

struct PlayerName {
    u8 id;
    char name[8];
};

struct PlayerPos {
    u8 id;
    vec2i pos;
};

struct PlayerAnim {
    u8 id;
    u8 index;
    PlayerAnim() = default;
    PlayerAnim(const u8 &id, const u8 &index)
        : id(id), index(index) {}
};

#pragma pack(pop)