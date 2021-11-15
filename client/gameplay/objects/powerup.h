#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

enum {
    POWERUP_FIRE, POWERUP_SKATE, POWERUP_BOMB, POWERUP_COUNT
};

class Powerup : public GameObject {
public:
    using GameObject::GameObject;

    Powerup(vec2i position);
    ~Powerup();
    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    inline int getType() { return type; }

private:
    StaticBody collider;
    int type = 0;
};