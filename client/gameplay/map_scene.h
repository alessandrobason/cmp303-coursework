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

#include <vector>
#include <tracelog.h>
#include <cthreads.h>

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

    void addObject(u8 id, vec2i pos, u8 type = 0);
    void removeObject(u32 id, vec2i pos);

    inline bool isReady() { lock_t lock(map_mutex); return map_id != -1; }

    inline void setMapId(int id) { lock_t lock(map_mutex); map_id = id; }
    inline std::vector<GameObject *> &getObjects() { return objects; }

    inline void setFinished(bool won) { is_finished = true; has_won = won; }

private:
    void actuallyRemove();

    Tilemap map;
    int map_id = -1;
    std::vector<GameObject *> objects;
    std::vector<GameObject *> to_add;
    std::vector<std::pair<u32, vec2i>> to_remove;

    cmutex_t map_mutex = NULL;
    std::vector<u8> players_to_add;

    Sprite win, lose;

    bool draw_colliders = false;
    bool is_finished = false;
    bool has_won = false;
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
