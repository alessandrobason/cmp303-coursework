#include "player.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>
#include <gameplay/objects/crate.h>
#include <gameplay/objects/powerup.h>

enum {
    STAND_DOWN, STAND_UP, STAND_LEFT, STAND_RIGHT,
    WALK_DOWN, WALK_UP, WALK_LEFT, WALK_RIGHT,
    ANIM_COUNT
};

enum {
    DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_NONE
};

enum {
    LOSING_ANIMATION, WINNING_ANIMATION
};

void Player::onInit() {
    type_id = getUniqueTypeId(this);
    sprite.load("res/player.json");
    sprite.origin = { 0, 16 };
    sprite.position = { 5 * 16, 6 * 16 };

    end_sprite.load("res/player_end.json");
    end_sprite.origin = { 8, 14 };

    collider.init(sprite.position, false);
    collider.setUserData(this);

    collider.setFixtureAsBox({ 16, 16 }, 0.f, 0.f, 1.f, LAYER_1);
}

void Player::onExit() {
    collider.cleanup();
}

void Player::onUpdate() {
    assert(type_id.id != 0 && unique_id.id != 0 && "didn't initialize ids");
    switch(cur_state) {
    case STATE_PLAYING: playingUpdate(); break;
    case STATE_WINNING: winningUpdate(); break;
    case STATE_LOSING:  losingUpdate();  break;
    }
}

void Player::onRender(bool is_debug) {
    switch(cur_state) {
    case STATE_PLAYING: drawSprite(sprite); break;
    case STATE_WINNING: 
    case STATE_LOSING:  drawSprite(end_sprite); break;
    }
    if(is_debug) {
        collider.render(RED);
    }
}

void Player::onHit() {
    cur_state = STATE_LOSING;
    end_sprite.position = sprite.position;
    end_sprite.play(LOSING_ANIMATION);
}

void Player::onWin() {
    cur_state = STATE_WINNING;
    end_sprite.position = sprite.position;
    end_sprite.play(WINNING_ANIMATION);
}

void Player::onPowerup(int type) {
    switch (type) {
    case POWERUP_FIRE:
        bomb_strength = min(bomb_strength + 1, max_bomb_strength);
        break;
    case POWERUP_SKATE:
        speed = min(speed + 3, max_speed);
        break;
    case POWERUP_BOMB:
        bombs = min(bombs + 1, max_bombs);
        break;
    default:
        warn("unrecognized powerup: %d", type);
    }
}

void Player::increaseBomb() {
    bomb_count--;
}

void Player::playingUpdate() {
    sprite.update();
    if(IsKeyPressed(KEY_Y)) {
        info("player data:");
        printf("\t\tbomb strength: %d\n", bomb_strength);
        printf("\t\tspeed: %d\n", speed);
        printf("\t\tmax bombs: %d\n", bombs);
        printf("\t\tbomb count: %d\n", bomb_count);
    }
    if(!is_locked) {
        updateMovement();
        if(IsKeyPressed(KEY_Z) && bomb_count < bombs) {
            u16 mask = collider.getBody()->GetFixtureList()->GetFilterData().categoryBits;
            vec2i pos = sprite.position;
            gmap.clampToCell(pos);
            gmap.addObject<Bomb>(gmap.positionToIndex(pos), false, pos, mask, bomb_strength);
            bomb_count++;
        }
    }
}

void Player::winningUpdate() {
    end_sprite.update();
}

void Player::losingUpdate() {
    if(IsKeyPressed(KEY_T)) {
        cur_state = STATE_PLAYING;
        return;
    }
    if(collider.getBody()->IsEnabled()) {
        collider.getBody()->SetEnabled(false);
    }
    end_sprite.updateCallback(
        [](Sprite &spr, void *udata) {
            ((GameObject *)udata)->setDead(true);
        }, 
        this
    );
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
    sprite.position = collider.getPosition();

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
