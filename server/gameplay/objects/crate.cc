#include "crate.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>
#include <gameplay/objects/powerup.h>

Crate::Crate(vec2i position) {
    sprite.position = position;
    sprite.origin = { 0, 5 };
}

Crate::~Crate() {
    collider.cleanup();
}

void Crate::onInit() {
    sprite.load("res/crate.png");

    collider.init(sprite.position, false);
    collider.setUserData(this);
    collider.setFixtureAsCircle(vec2i(8), 8, LAYER_7);
}

void Crate::onUpdate() {
    GameObject::onUpdate();
}

void Crate::onRender(bool is_debug) {
    GameObject::onRender(is_debug);
    if(is_debug) {
        collider.render(LIME);
    }
}

void Crate::onHit() {
    dead = true;
    int r = rand() % 10;
    if(r < 5) {
        info(">> a");
        gmap.addObject<Powerup, vec2i>(gmap.positionToIndex(sprite.position), true, std::move(sprite.position));
        info(">> b");
    }
}