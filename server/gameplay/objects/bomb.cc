#include "bomb.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>

Bomb::Bomb(vec2i position, u16 source_layer, int bomb_strength) {
    sprite.position = position;
    mask = source_layer;
    strength = bomb_strength;
}

Bomb::~Bomb() {
    collider.cleanup();
}

void Bomb::onInit() {
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
    if(!is_solid) {
        if(!is_inside) {
            makeSolid();
        }
        is_inside = false;
    }

    timer -= GetFrameTime();
    if(timer <= 0.f) {
        dead = true;
        increasePlayerBombs();
        gmap.setId(gmap.positionToIndex(sprite.position), 0);
        gmap.addExplosion(sprite.position, vec2i::zero(), 0);
        gmap.addExplosion(sprite.position, { -1,  0 }, strength);
        gmap.addExplosion(sprite.position, {  1,  0 }, strength);
        gmap.addExplosion(sprite.position, {  0, -1 }, strength);
        gmap.addExplosion(sprite.position, {  0,  1 }, strength);
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
    
    is_solid = true;
}

void Bomb::increasePlayerBombs() {
    auto &objects = gmap.getObjects();
    u32 player_id = Player::id();
    for(auto &obj : objects) {
        if(obj->getTypeId() == player_id) {
            Player *p = (Player *)obj;
            
            u16 pmask = p->getCollider().getMask();
            if(pmask == mask) {
                warn("FOUND PLAYER");
                p->increaseBomb();
            }
        }
    }
}