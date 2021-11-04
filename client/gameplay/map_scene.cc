#include "map_scene.h"

#include <algorithm>
#include <tracelog.h>

#include <core/config.h>
#include <gameplay/objects/player.h>

MapScene gmap;

bool sortFn(const ptr<GameObject> &a, const ptr<GameObject> &b) {
    return *a < *b;
}

MapScene::~MapScene() {
}

void MapScene::onInit() {
    map.loadFromJSON("res/tiled/map_1.json");

    objects.emplace_back(std::make_unique<Player>());

    for(auto &obj : objects) {
        obj->onInit();
    }
}

void MapScene::onExit() {
    for(auto &obj : objects) {
        obj->onExit();
    }
    objects.clear();
    map.onExit();
}

void MapScene::onUpdate() {
    g_world.Step(1.f / 60.f, 6, 2);
    handleCollisions();

    if(IsKeyPressed(KEY_O)) {
        draw_colliders = !draw_colliders;
    }

    map.update();
    for(auto &obj : objects) {
        obj->onUpdate();
    }
    
    for(auto &drop : to_drop) {
        objects.emplace_back(std::make_unique<Bomb>(drop.first, drop.second));
        objects.back()->onInit();
    }

    to_drop.clear();

    std::sort(objects.begin(), objects.end(), sortFn);
}

void MapScene::onRender() {
    drawTilemapUnder(map);
        if(draw_colliders) {
            map.colliders.render(ORANGE);
        }
        for(auto &obj : objects) {
            obj->onRender(draw_colliders);
        }
    drawTilemapOver(map);
}

void MapScene::dropBomb(vec2i position, u16 source_layer) {
    // clamp position to cell
    int res = position.x % 16;
    if(res != 0) {
        if(res > 8) res -= 16;
        position.x -= res;
    }
    res = position.y % 16;
    if(res != 0) {
        if(res > 8) res -= 16;
        position.y -= res;
    }

    // check if there is already a bomb in the same place

    for(auto &drop : to_drop) {
        if(drop.first == position) return;
    }

    u32 bomb_id = getUniqueId<Bomb>();
    for(auto &obj : objects) {
        if(obj->getId() == bomb_id) {
            if(position == obj->getPosition()) {
                return;
            }
        }
    }

    to_drop.emplace_back(position, source_layer);
}

void MapScene::handleCollisions() {
    b2Contact *contact = g_world.GetContactList();
	int contact_count = g_world.GetContactCount();

    for(int i = 0; i < contact_count; ++i) {
        if(contact->IsTouching()) {
			b2Body *body_a = contact->GetFixtureA()->GetBody();
			b2Body *body_b = contact->GetFixtureB()->GetBody();

			((StaticBody *)body_a->GetUserData().pointer)->collision(body_b);
			((StaticBody *)body_b->GetUserData().pointer)->collision(body_a);
        }

		contact = contact->GetNext();
    }
}