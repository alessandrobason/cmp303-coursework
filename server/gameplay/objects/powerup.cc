#include "powerup.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>

Powerup::Powerup(vec2i position) {
    type = rand() % POWERUP_COUNT;
    sprite.position = position;
}

Powerup::~Powerup() {
    collider.cleanup();
}

void Powerup::onInit() {
    sprite.load("res/powerups.png", {16 * type, 0, 16, 16});

    collider.init(sprite.position);
    collider.setFixtureAsCircle(8, 6, 0.f, 0.f, 1.f, LAYER_7, ALL_LAYERS, true);

    collider.setUserData(this);
    collider.setCollisionResponse(
        [](b2Body *body, void *udata) {
            u16 layer = body->GetFixtureList()->GetFilterData().categoryBits;
            if(layer >= LAYER_1 && layer <= LAYER_4) {
                warn("powerup!");
                Powerup *self = ((Powerup *)udata);
                self->setDead(true);
                Player *p = (Player *)((StaticBody *)body->GetUserData().pointer)->getUserData();
                p->onPowerup(self->getType());
            }
        }
    );
}

void Powerup::onUpdate() {
    GameObject::onUpdate();
}

void Powerup::onRender(bool is_debug) {
    GameObject::onRender();
    if(is_debug) {
        collider.render(YELLOW);
    }
}

void Powerup::onHit() {
    dead = true;
}