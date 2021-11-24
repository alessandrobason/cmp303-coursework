#include "player.h"

#include <tracelog.h>
#include <core/assets.h>
#include <gameplay/map_scene.h>
#include <gameplay/objects/crate.h>
#include <gameplay/objects/powerup.h>
#include <utils/interpolation.h>
#include <net/net.h>

Player players[max_players];
int player_count = 0;

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

void Player::setName(char newname[8]) {
    memcpy(name, newname, sizeof(name));
    name[sizeof(name)-1] = '\0';
}

void Player::addPlayer(u8 id, bool new_is_main) {
    constexpr vec2i positions[max_players] = {
        {  5 * 16,  6 * 16 },
        { 19 * 16,  6 * 16 },
        {  5 * 16, 14 * 16 },
        { 19 * 16, 14 * 16 }
    };
    constexpr Color colors[max_players] = {
        WHITE,
        { 102, 225, 0, 255 },
        { 225,  89, 0, 255 },
        { 208, 0, 225, 255 },
    };

    player_id = id;
    sprite.position = positions[player_id];
    sprite.color = colors[player_id];
    end_sprite.color = colors[player_id];
    is_main = new_is_main;
    player_count++;

    gmap.getObjects().emplace_back(this);
    info("player %d initialized", player_id);
}

void Player::onInit() {
    sprite.load("res/player.json");
    sprite.origin = { 0, 16 };

    end_sprite.load("res/player_end.json");
    end_sprite.origin = { 8, 14 };

    player_mtx = mtxInit();
    assert(mtxValid(player_mtx));
    
    next_pos = sprite.position;
    cur_state = STATE_PLAYING;
}

void Player::onExit() {
    sprite.destroy();
    end_sprite.destroy();

    mtxDestroy(player_mtx);
    player_mtx = NULL;
}

void Player::onUpdate() {
    assert(getTypeId() != 0 && unique_id.id != 0 && "didn't initialize ids");
    {
        lock_t lock(player_mtx);
        if (next_state != -1) {
            switch (next_state) {
            case STATE_WINNING: onWin(); break;
            case STATE_LOSING:  onHit(); break;
            }
            next_state = -1;
        }
    }
    switch(cur_state) {
    case STATE_PLAYING: playingUpdate(); break;
    case STATE_WINNING: winningUpdate(); break;
    case STATE_LOSING:  losingUpdate();  break;
    }
}

void Player::onRender(bool is_debug) {
    switch(cur_state) {
    case STATE_PLAYING: drawSprite(sprite);     break;
    case STATE_WINNING: 
    case STATE_LOSING:  drawSprite(end_sprite); break;
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
    gmap.setFinished(is_main);
}

void Player::setPosition(vec2i newpos) {
    if (!mtxValid(player_mtx)) return;
    lock_t lock(player_mtx);
    next_pos = newpos;
}

void Player::setAnimation(u8 new_anim) {
    lock_t lock(player_mtx);
    next_anim = new_anim;
}

void Player::setState(u8 new_state) {
    lock_t lock(player_mtx);
    next_state = new_state;
}

void Player::playingUpdate() {
    sprite.update();
    if(!is_locked && is_main) {
        updateMovement();
        if(IsKeyPressed(KEY_Z)) {
            lock_t lock(player_mtx);
            Message<vec2i> msg {
                MSG_BOMB_REQ, sprite.position
            };
            net.sendMessage(msg);
        }
    }
    {
        lock_t lock(player_mtx);
        if (lerp_time >= 0.f) {
            sprite.position = lerp<linear>((vec2f)lerp_pos, (vec2f)next_pos, lerp_time);
            if (lerp_time >= 1.f) {
                lerp_time = -1.f;
            }
            else {
                lerp_time += GetFrameTime() * 10.f;
            }
        }
        else {
            i32 diff = (next_pos - sprite.position).mag();
            if (diff > 10) {
                lerp_pos = sprite.position;
                lerp_time = 0.f;
            }
            else {
                sprite.position = next_pos;
            }
        }
        if (next_anim != sprite.cur_animation) {
            playAnimation(next_anim);
        }
    }
}

void Player::winningUpdate() {
    end_sprite.update();
}

void Player::losingUpdate() {
    end_sprite.update();
}

void Player::updateMovement() {
    vec2i dir {
        IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT),
        IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)
    };

    if(dir != old_dir) {
        // send direction
        Message<vec2b> dir_msg { MSG_DIR, dir };
        net.sendMessage(dir_msg);
    }

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
