#include "map_scene.h"

#include <algorithm>
#include <tracelog.h>
#include <time.h>

#include <gameplay/objects/explosion.h>

MapScene gmap;

bool sortFn(const ptr<GameObject> &a, const ptr<GameObject> &b) {
    return *a < *b;
}

MapScene::~MapScene() {
}

void MapScene::onInit() {
    srand(time(NULL));
    int r = rand() % 3;
    // r = 0;
    map.loadFromJSON(TextFormat("res/tiled/map_%d.json", r));
    // map.loadFromJSON("res/tiled/map_1.json");
    int under = (map.layers_under - 1) * map.size.x * map.size.y;
    obj_map.resize(map.size.x * map.size.y);
    for(int y = 0; y < map.size.y; ++y) {
        for(int x = 0; x < map.size.x; ++x) {
            int i = x + y * map.size.x;
            if(map.tiles[under + i] >= 0) {
                obj_map[i].id = -1;
            }
        }
    }
    placeCrates();
}

void MapScene::onEnter() {
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
}

void MapScene::onDestroy() {
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

    actuallyRemove();

    for(auto &add : to_add) {
        add->onInit();
        objects.emplace_back(add);
    }

    to_add.clear();

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

void MapScene::addExplosion(vec2i position, vec2i direction, int strength) {
    clampToCell(position);
    vec2i next_pos = position + (direction * Config::get().tile_size);
    position = next_pos / Config::get().tile_size;
    u32 index = position.x + position.y * map.size.x;
    TypeId type = obj_map[index];

    if(type.id == 0) {
        addObject<Explosion>(index, true, next_pos, direction, strength);
    }
    else if(type == getUniqueTypeId<Crate>()) {
        for(auto &obj : objects) {
            if(obj->getPosition() == next_pos) {
                ((Crate *)obj.get())->onHit();
                break;
            }
        }
        addObject<Explosion>(index, true, next_pos, direction, 0);
    }
}

void MapScene::removeCollider(StaticBody *ptr) {
    body_to_remove.emplace_back(ptr);
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

void MapScene::actuallyRemove() {
    size_t size = objects.size();
    for(size_t i = 0; i < size; ++i) {
        auto &obj = objects[i];
        if(obj->isDead()) {
            setId(positionToIndex(obj->getPosition()), 0);

            obj->onExit();
            i--;
            size--;
            obj.swap(objects[size]);
        }
    }
    
    objects.resize(size);

    for(auto &body : body_to_remove) {
        body->cleanup();
    }

    body_to_remove.clear();
}

void MapScene::placeCrates() {
    constexpr int ystart = 6;
    constexpr int yend   = 15;
    constexpr int xstart = 5;
    constexpr int xend   = 20;

    constexpr vec2i skip[] = {
        {  5,  6 }, {  6,  6 }, {  5,  7 },
        { 18,  6 }, { 19,  6 }, { 19,  7 },
        {  5, 13 }, {  6, 14 }, {  5, 14 },
        { 18, 14 }, { 19, 14 }, { 19, 13 },
    };

    for(int y = ystart; y < yend; ++y) {
        for(int x = xstart; x < xend; ++x) {
            vec2i v { x, y };
            bool should_skip = false;
            for(auto &s : skip) {
                if(s == v) {
                    should_skip = true;
                    break;
                }
            }
            if(should_skip) continue;
            u32 index = x + y * map.size.x;
            if(obj_map[index].id == 0) {
                int r = rand() % 10;
                if(r < 8) {
                    addObject<Crate>(index, true, v * Config::get().tile_size);
                }
            }
        }
    }
}