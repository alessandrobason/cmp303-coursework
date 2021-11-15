#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Player : public GameObject {
    enum {
        STATE_PLAYING, STATE_WINNING, STATE_LOSING
    };

public:
    using GameObject::GameObject;

    void onInit() override;
    void onExit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    void onHit();
    void onWin();
    void onPowerup(int type);
    void increaseBomb();

    inline DynamicBody &getCollider() { return collider; }

private:
    void playingUpdate();
    void winningUpdate();
    void losingUpdate();

    void updateMovement();

    void playAnimation(u32 index);
    void setIdleAnim(vec2i dir);
    void setWalkingAnim(vec2i dir);
    int dirToInt(vec2i dir);

    int bomb_strength = 0;
    int bombs = 1;
    int bomb_count = 0;

    int max_speed = 18;
    int max_bombs = 6;
    int max_bomb_strength = 8;

    i32 speed = 9;
    DynamicBody collider;

    bool is_locked = false;
    vec2i old_dir;

    Sprite end_sprite;

    int cur_state = STATE_PLAYING;
};