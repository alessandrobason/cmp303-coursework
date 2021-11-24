#include "player.h"

#include <tracelog.h>
#include <core/assets.h>
#include <gameplay/map_scene.h>
#include <gameplay/objects/crate.h>
#include <gameplay/objects/powerup.h>
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

void Player::addPlayer(u8 id, bool is_main) {
    if(player_count >= max_players) return;

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
    player_count++;
    
    gmap.getObjects().emplace_back(this);
    info("player %d initialized", player_id);
}

void Player::onInit() {
    sprite.load("res/player.json");
    sprite.origin = { 0, 16 };

    end_sprite.load("res/player_end.json");
    end_sprite.origin = { 8, 14 };

    collider.init(sprite.position, false);
    collider.setUserData(this);
    collider.setFixtureAsBox({ 16, 16 }, 0.f, 0.f, 1.f, LAYER_1, physics_layers(ALL_LAYERS ^ LAYER_1));
    //collider.setFixtureAsBox({ 16, 16 }, 0.f, 0.f, 1.f, vec2f::zero(), 0.f, LAYER_1, physics_layers(ALL_LAYERS ^ LAYER_1));

    player_mtx = mtxInit();
    assert(mtxValid(player_mtx));

    cur_state = STATE_PLAYING;
}

void Player::onExit() {
    collider.cleanup();

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
    case STATE_PLAYING: drawSprite(sprite); break;
    case STATE_WINNING: 
    case STATE_LOSING:  drawSprite(end_sprite); break;
    }
    if(is_debug) {
        collider.render(RED);
    }
}

void Player::onHit() {
    if (cur_state != STATE_PLAYING) return;
    cur_state = STATE_LOSING;
    end_sprite.position = sprite.position;
    end_sprite.play(LOSING_ANIMATION);
    Message<PlayerAnim> msg{ MSG_STATE, { (u8)player_id, (u8)STATE_LOSING} };
    net.sendMessage(msg, SEND_ALL);
    player_count--;
    warn("players left: %d", player_count);
    if (player_count == 1) {
        for (int i = 0; i < max_players; ++i) {
            if (players[i].isAlive()) {
                players[i].onWin();
                break;
            }
        }
    }
}

void Player::onWin() {
    if (cur_state != STATE_PLAYING) return;
    cur_state = STATE_WINNING;
    end_sprite.position = sprite.position;
    end_sprite.play(WINNING_ANIMATION);
    Message<PlayerAnim> msg{ MSG_STATE, { (u8)player_id, (u8)STATE_WINNING} };
    net.sendMessage(msg, SEND_ALL);
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
    lock_t lock(player_mtx);
    info("player %d bomb count %d", player_id, bomb_count);
    if(bomb_count > 0) bomb_count--;
    info("player %d bomb count %d", player_id, bomb_count);
}

void Player::setPosition(vec2i newpos) {
    sprite.position = newpos;
}

void Player::setState(u8 new_state) {
    lock_t lock(player_mtx);
    next_state = new_state;
}

void Player::updateDirection(vec2i newdir) {
    lock_t lock(player_mtx);
    if (cur_state == STATE_PLAYING) {
        next_dir = newdir;
    }
}

void Player::playingUpdate() {
    sprite.update();
    if(!is_locked) {
        updateMovement();
    }
    {
        lock_t lock(player_mtx);

        if(next_dir != old_dir) {
            if(next_dir == vec2i::zero()) {
                setIdleAnim(old_dir);
            }
            else {
                setWalkingAnim(next_dir);
            }
        }

        old_dir = next_dir;

        b2Vec2 vel = {(f32)next_dir.x, (f32)next_dir.y};
        vel *= speed;

        collider.getBody()->SetLinearVelocity(vel);

        if (should_drop_bomb) {
            u16 mask = collider.getMask();
            vec2i pos = sprite.position;
            gmap.clampToCell(pos);
            bool added = gmap.addObject<Bomb>(gmap.positionToIndex(pos), false, pos, mask, bomb_strength);
            if(added) {
                bomb_count++;
            }
            should_drop_bomb = false;
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
    sprite.position = collider.getPosition();
    if (sprite.position != old_pos) {
        Message<PlayerPos> msg {
            MSG_POS, { (u8)player_id, sprite.position }
        };
        net.sendMessage(msg, SEND_ALL);
    }
    old_pos = sprite.position;
}

void Player::dropBomb() {
    lock_t lock(player_mtx);
    if (bomb_count < bombs) {
        info("%d -> %d < %d", player_id, bomb_count, bombs);
        should_drop_bomb = true;
    }
}

void Player::playAnimation(u32 index) {
    sprite.cur_animation = index;
    sprite.animations[index].timer = 0.f;
    sprite.animations[index].cur_image = 0;
    Message<PlayerAnim> msg{ MSG_ANIM, { (u8)player_id, (u8)index } };
    net.sendMessage(msg, SEND_ALL);
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
