#include "bomb.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>

Bomb::Bomb(vec2i position, u16 source_layer) {
    sprite.position = position;
    mask = source_layer;
}

void Bomb::onInit() {
    unique_id = getUniqueId(this);
    sprite.load("res/bomb.png", 1, 4, 0.2f);
    collider.init();

    collider.setUserData(this);
    collider.setCollisionResponse(
        [](b2Body *body, void *udata) {
            ((Bomb *)udata)->setIsInside(true);
        }
    );
    
    vec2i halfsz = sprite.size / 2;
    collider.setFixtureAsCircle(sprite.position + halfsz, halfsz.x, LAYER_5, physics_layers(ALL_LAYERS ^ mask));
    collider.setFixtureAsCircle(sprite.position + halfsz, halfsz.x - 1, 0.2f, 0.f, 0.f, LAYER_5, physics_layers(mask), true);
}

void Bomb::onUpdate() {
    GameObject::onUpdate();
    if(mask) {
        if(!is_inside) {
            makeSolid();
        }
        is_inside = false;
    }

}

void Bomb::onRender(bool is_debug) {
    GameObject::onRender();
    if(is_debug) {
        collider.render(BLUE);
    }
}

void Bomb::makeSolid() {
    b2Fixture *fix = collider.getBody()->GetFixtureList();
    while(fix) {
        b2Filter filter = fix->GetFilterData();
        if(fix->IsSensor()) {
            b2Fixture *next = fix->GetNext();
            collider.getBody()->DestroyFixture(fix);
            fix = next;
            continue;
        }
        filter.maskBits |= mask;
        fix->SetFilterData(filter);
        fix = fix->GetNext();
    }
    mask = 0;
}