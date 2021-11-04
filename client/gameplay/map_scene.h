#pragma once

#include <core/scene.h>
#include <core/tilemap.h>
#include <gameplay/game_object.h>
#include <gameplay/objects/player.h>
#include <gameplay/objects/bomb.h>
#include <utils/ptr.h>

#include <vector>

class MapScene : public Scene {
public:
    ~MapScene() override;
    void onInit() override;
    void onExit() override;
    void onUpdate() override;
    void onRender() override;

    void dropBomb(vec2i position, u16 source_layer);

    inline std::vector<ptr<GameObject>> &getObjects() { return objects; }

private:
    void handleCollisions();

    Tilemap map;
    std::vector<ptr<GameObject>> objects;
    std::vector<std::pair<vec2i, u16>> to_drop;
    bool draw_colliders = false;
};

extern MapScene gmap;