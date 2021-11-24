#pragma once

#include <core/scene.h>
#include <core/tilemap.h>
#include <core/config.h>
#include <gameplay/game_object.h>
#include <gameplay/objects/player.h>
#include <gameplay/objects/bomb.h>
#include <gameplay/objects/crate.h>
#include <gameplay/objects/powerup.h>
#include <utils/ptr.h>
#include <net/net.h>

#include <vector>
#include <tracelog.h>
#include <cthreads.h>

constexpr int map_count = 8;

class MapScene : public Scene {
public:
    ~MapScene() override;
    void onInit() override;
    void onEnter() override;
    void onExit() override;
    void onDestroy() override;
    void onUpdate() override;
    void onRender() override;

    u32 positionToIndex(vec2i pos);
    void clampToCell(vec2i &pos);
    bool isDestructable(u32 type);

    template<typename T, typename ...Targs>
    bool addObject(u32 index, bool skip_check, Targs &&...args);
    template<>
    bool addObject<Powerup, vec2i>(u32 index, bool skip_check, vec2i &&pos);

    void setId(u32 index, u32 value);
    void removeCollider(StaticBody *ptr);

    inline void setMapId(int new_id) { map_id = new_id; }
    inline int getMapId() { return map_id; }

    void addExplosion(vec2i position, vec2i direction, int strength);

    inline std::vector<GameObject *> &getObjects() { return objects; }
    inline std::vector<vec2i> &getCrates() { return crates;  }

private:
    void handleCollisions();
    void actuallyRemove();
    void placeCrates();

    Tilemap map;
    int map_id = -1;
    std::vector<u32> obj_map;
    std::vector<GameObject *> objects;

    std::vector<GameObject *> to_add;
    std::vector<StaticBody *> body_to_remove;

    cmutex_t map_mutex;
    std::vector<vec2i> crates;
    bool draw_colliders = false;
};

extern MapScene gmap;

// == INLINE FUNCTIONS ================================================

inline u32 MapScene::positionToIndex(vec2i pos) {
    pos /= Config::get().tile_size;
    return pos.x + pos.y * map.size.x;
}

inline void MapScene::clampToCell(vec2i &pos) {
    int res = pos.x % 16;
    if(res != 0) {
        if(res > 8) res -= 16;
        pos.x -= res;
    }
    res = pos.y % 16;
    if(res != 0) {
        if(res > 8) res -= 16;
        pos.y -= res;
    }
}


inline bool MapScene::isDestructable(u32 type) {
    return type == 0 || type == Crate::id();
}

template<typename T, typename ...Targs>
bool MapScene::addObject(u32 index, bool skip_check, Targs&&... args) {
    if(index >= obj_map.size()) {
        obj_map.resize(index + 1);
    }
    // if there is already something in that spot, return early
    else if(!skip_check && obj_map[index] != 0) {
        warn("occupied %u", T::id());
        return false;
    }

    obj_map[index] = T::id();
    T *obj = new T(args...);
    to_add.emplace_back(obj);

    Message<AddRemMsg> msg{ MSG_OBJ_ADD, { (u8)T::id(), obj->getPosition() } };
    net.sendMessage(msg, SEND_ALL);

    return true;
}


template<>
bool MapScene::addObject<Powerup, vec2i>(u32 index, bool skip_check, vec2i &&pos) {
    if (index >= obj_map.size()) {
        obj_map.resize(index + 1);
    }
    // if there is already something in that spot, return early
    else if (!skip_check && obj_map[index] != 0) {
        warn("occupied %u", Powerup::id());
        return false;
    }

    obj_map[index] = Powerup::id();
    Powerup *obj = new Powerup(pos);
    to_add.emplace_back(obj);

    debug("powerup type: %d", obj->getType());

    Message<AddPowerup> msg{
        MSG_POW_ADD,
        { (u8)obj->getType(), obj->getPosition() }
    };
    net.sendMessage(msg, SEND_ALL);

    return true;
}

inline void MapScene::setId(u32 index, u32 value) {
#ifndef NDEBUG
    if(index >= obj_map.size()) {
        fatal("index too large: %u -> %zu", index, obj_map.size());
    }
#endif
    obj_map[index] = value;
}
