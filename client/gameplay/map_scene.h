#pragma once

#include <core/scene.h>
#include <core/tilemap.h>
#include <core/config.h>
#include <gameplay/game_object.h>
#include <gameplay/objects/player.h>
#include <gameplay/objects/bomb.h>
#include <gameplay/objects/crate.h>
#include <utils/ptr.h>

#include <vector>
#include <tracelog.h>

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
    bool isDestructable(TypeId type);

    template<typename T, typename ...Targs>
    void addObject(u32 index, bool skip_check, Targs &&...args);

    // void removeObject(GameObject *ptr);
    void setId(u32 index, TypeId value);
    void removeCollider(StaticBody *ptr);

    void addExplosion(vec2i position, vec2i direction, int strength);

    inline std::vector<ptr<GameObject>> &getObjects() { return objects; }

private:
    void handleCollisions();
    void actuallyRemove();
    void placeCrates();

    Tilemap map;
    std::vector<TypeId> obj_map;
    std::vector<ptr<GameObject>> objects;

    std::vector<GameObject *> to_add;
    std::vector<StaticBody *> body_to_remove;

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


inline bool MapScene::isDestructable(TypeId type) {
    return type.id == 0 || type == getUniqueTypeId<Crate>();
}

template<typename T, typename ...Targs>
void MapScene::addObject(u32 index, bool skip_check, Targs&&... args) {
    if(index >= obj_map.size()) {
        obj_map.resize(index + 1);
    }
    // if there is already something in that spot, return early
    else if(!skip_check && obj_map[index].id != 0) {
        return;
    }

    obj_map[index] = getUniqueTypeId<T>();
    to_add.emplace_back(new T(args...));
}

inline void MapScene::setId(u32 index, TypeId value) {
#ifndef NDEBUG
    if(index >= obj_map.size()) {
        fatal("index too large: %u -> %zu", index, obj_map.size());
    }
#endif
    obj_map[index] = value;
}