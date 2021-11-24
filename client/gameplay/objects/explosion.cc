#include "explosion.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>
#include <gameplay/objects/crate.h>

void Explosion::onInit() {
    sprite.origin = { 16, 16 };
    sprite.load("res/bomb_explosion.png", 2, 4, 0.08f);
    sprite.animations[0].repeat = false;
    sprite.animations[1].repeat = false;
}

void Explosion::onUpdate() {
    sprite.updateCallback(
        [](Sprite &spr, void *udata) {
            auto self = (Explosion *)udata;
            if (spr.cur_animation == 0) {
                spr.cur_animation = 1;
                return;
            }
        },
        this
    );
}
