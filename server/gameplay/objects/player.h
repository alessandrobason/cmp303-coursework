#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>
#include <cthreads.h>

constexpr int max_players = 4;

class Player : public GameObject {
public:
    enum {
        STATE_NONE, STATE_PLAYING, STATE_WINNING, STATE_LOSING
    };

    using GameObject::GameObject;

    inline static u32 id() { return 4; }

    void setName(char newname[8]);
    inline const char *getName() const { return name; };

    void addPlayer(u8 id, bool is_main);

    void onInit() override;
    void onExit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    void onHit();
    void onWin();
    void onPowerup(int type);
    void increaseBomb();

    void setPosition(vec2i newpos);
    void setState(u8 new_state);

    void updateDirection(vec2i dir);
    void dropBomb();

    inline bool isAlive() { return cur_state == STATE_PLAYING; }

    inline DynamicBody &getCollider() { return collider; }
    inline u32 getTypeId() const override { return Player::id(); };

private:
    void playingUpdate();
    void winningUpdate();
    void losingUpdate();

    void updateMovement();

    void playAnimation(u32 index);
    void setIdleAnim(vec2i dir);
    void setWalkingAnim(vec2i dir);
    int dirToInt(vec2i dir);

    char name[8] = {0};

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
    vec2i next_dir;
    vec2i old_pos;
    int next_state = -1;

    Sprite end_sprite;

    int cur_state = STATE_NONE;

    int player_id = -1;
    cmutex_t player_mtx = NULL;
    bool should_drop_bomb = false;
};

extern Player players[max_players];
extern int player_count;