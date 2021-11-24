#include "explosion.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>
#include <gameplay/objects/crate.h>

Explosion::Explosion(vec2i position, vec2i direction, int strength)
    : direction(direction), strength(strength) {
    sprite.origin = { 16, 16 };
    sprite.position = position;
}

Explosion::~Explosion() {
    collider.cleanup();
}

void Explosion::onInit() {
    sprite.load("res/bomb_explosion.png", 2, 4, 0.08f);
    sprite.animations[0].repeat = false;
    sprite.animations[1].repeat = false;

    collider.setUserData(this);
    collider.setCollisionResponse(
        [](b2Body *body, void *udata) {
            u16 layer = body->GetFixtureList()->GetFilterData().categoryBits;
            if(layer >= LAYER_1 && layer <= LAYER_4) {
                Player *player = (Player *)((StaticBody *)body->GetUserData().pointer)->getUserData();
                player->onHit();
            }
        }
    );

    collider.init(sprite.position + vec2i::one(), false);
    collider.setFixtureAsBox({14, 14}, 0.f, 0.f, 1.f, NO_LAYER, ALL_LAYERS, true);
}

void Explosion::onUpdate() {
    sprite.updateCallback(
        [](Sprite &spr, void *udata) {
            auto self = (Explosion *)udata;
            if(spr.cur_animation == 0) {
                spr.cur_animation = 1;
                gmap.removeCollider(self->getCollider());
                return;
            }
            self->setDead(true);
        }, 
        this
    );
    if(strength) {
        timer -= GetFrameTime();
        if(timer <= 0.f) {
            gmap.addExplosion(sprite.position, direction, strength - 1);
            strength = 0;
        }
    }
}

void Explosion::onRender(bool is_debug) {
    GameObject::onRender(is_debug);
    if(is_debug) {
        collider.render(PINK);
    }
}
