#include "player.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>

enum {
    STAND_DOWN, STAND_UP, STAND_LEFT, STAND_RIGHT,
    WALK_DOWN, WALK_UP, WALK_LEFT, WALK_RIGHT,
    ANIM_COUNT
};

enum {
    DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_NONE
};

void Player::onInit() {
    unique_id = getUniqueId(this);
    sprite.load("res/player.json");
    sprite.origin = { 0, 16 };
    sprite.position = { 5 * 16, 6 * 16 };
    collider.init(sprite.position);
    collider.setFixtureAsBox({ 16, 16 }, 0.3f, 0.f, 1.f, LAYER_1);
}

void Player::onUpdate() {
    GameObject::onUpdate();
    if(!is_locked) {
        updateMovement();
        if(IsKeyPressed(KEY_Z)) {
            u16 mask = collider.getBody()->GetFixtureList()->GetFilterData().categoryBits;
            gmap.dropBomb(sprite.position, mask);
        }
    }
}

void Player::onRender(bool is_debug) {
    GameObject::onRender();
    if(is_debug) {
        collider.render(RED);
    }
}

void Player::updateMovement() {
    vec2i dir {
        IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        IsKeyDown(KEY_S) - IsKeyDown(KEY_W)
    };

    if(dir != old_dir) {
        if(dir == vec2i::zero()) {
            setIdleAnim(old_dir);
        }
        else {
            setWalkingAnim(dir);
        }
    }

    b2Vec2 vel = {(f32)dir.x, (f32)dir.y};
    vel *= speed;

    collider.getBody()->SetLinearVelocity(vel);
    const b2Vec2 &new_pos = collider.getBody()->GetPosition();
    sprite.position.x = (i32)round(new_pos.x);
    sprite.position.y = (i32)round(new_pos.y);

    old_dir = dir;
}

void Player::playAnimation(u32 index) {
    sprite.cur_animation = index;
    sprite.animations[index].timer = 0.f;
    sprite.animations[index].cur_image = 0;
}

void Player::setIdleAnim(vec2i dir) {
    int direction = dirToInt(dir);
    if(direction == DIR_NONE) return;
    playAnimation(STAND_DOWN + direction);
}

void Player::setWalkingAnim(vec2i dir) {
    int direction = dirToInt(dir);
    if(direction == DIR_NONE) return;
    playAnimation(WALK_DOWN + direction);
}

int Player::dirToInt(vec2i dir) {
    if(dir.y < 0) return DIR_UP;
    if(dir.y > 0) return DIR_DOWN;
    if(dir.x < 0) return DIR_LEFT;
    if(dir.x > 0) return DIR_RIGHT;
    return DIR_NONE;
}
