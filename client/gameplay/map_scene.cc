#include "map_scene.h"

#include <algorithm>
#include <tracelog.h>
#include <time.h>

#include <utils/interpolation.h>
#include <gameplay/objects/explosion.h>
#include <gameplay/objects/powerup.h>

MapScene gmap;

bool sortFn(const GameObject *a, const GameObject *b) {
    return *a < *b;
}

MapScene::~MapScene() {
    mtxDestroy(map_mutex);
}

void MapScene::onInit() {
    map_mutex = mtxInit();

    win.load("res/win.png");
    lose.load("res/lose.png");

    int res_w = Config::get().resolution.x;

    win.position.x  = (res_w - win.size.x) / 2;
    lose.position.x = (res_w - lose.size.x) / 2;
}

void MapScene::onEnter() {
    assert(map_id != -1);
    map.loadFromJSON(TextFormat("res/tiled/map_%d.json", map_id));
    
    for (auto &obj : objects) {
        obj->onInit();
    }
}

void MapScene::onExit() {
    for (auto &obj : objects) {
        obj->onExit();
    }
    objects.clear();
}

void MapScene::onDestroy() {
    map.onExit();
}

void MapScene::onUpdate() {
    map.update();
    for (auto &obj : objects) {
        if (obj->getTypeId() == Bomb::id()) {
            int a = 0;
        }
        obj->onUpdate();
    }

    actuallyRemove();

    {
        lock_t lock(map_mutex);

        for (auto &add : to_add) {
            add->onInit();
            objects.emplace_back(add);
        }

        to_add.clear();
    }

    std::sort(objects.begin(), objects.end(), sortFn);
}

void MapScene::onRender() {
    drawTilemapUnder(map);
    for (auto &obj : objects) {
        obj->onRender(draw_colliders);
    }
    drawTilemapOver(map);

    {
        lock_t lock(map_mutex);
        if(is_finished) {
            static float t = 0.f;

            if(has_won) {
                win.position.y = lerp<easeOutElastic>(-20.f, 132.f, t);
                drawSprite(win);
            }
            else {
                lose.position.y = lerp<easeOutElastic>(-20.f, 132.f, t);
                drawSprite(lose);
            }

            t += GetFrameTime() * 0.2f;
        }
    }
}

void MapScene::addObject(u8 id, vec2i pos, u8 type) {
    lock_t lock(map_mutex);
    switch (id) {
    case Bomb::id():
        to_add.emplace_back(new Bomb(pos));
        break;
    case Crate::id():
        to_add.emplace_back(new Crate(pos));
        break;
    case Explosion::id():
        to_add.emplace_back(new Explosion(pos));
        break;
    case Powerup::id(): {
        Powerup *p = new Powerup(pos);
        p->setType(type);
        to_add.emplace_back(p);
        break;
    }
    default: 
        err("unknown id: %u", id);
    }
}

void MapScene::removeObject(u32 id, vec2i pos) {
    lock_t lock(map_mutex);
    for (size_t i = 0; i < objects.size(); ++i) {
        auto &obj = objects[i];
        if (!obj->isDead() &&
             obj->getTypeId() == id &&
             obj->getPosition() == pos) 
        {
            obj->setDead(true);
            break;
        }
    }
}

void MapScene::actuallyRemove() {
    // -- Remove objects ---------------------------------
    
    {
        lock_t lock(map_mutex);
        i64 size = objects.size();
        for (i64 i = 0; i < size; ++i) {
            auto &obj = objects[i];
            if (obj->isDead()) {
                obj->onExit();
                if (obj->getTypeId() != Player::id()) {
                    delete obj;
                }
                objects[i] = objects[size - 1];
                objects[size - 1] = nullptr;
                //std::swap(objects[i], objects[size - 1]);
                i--;
                size--;
            }
        }

        objects.resize(size);
    }

}