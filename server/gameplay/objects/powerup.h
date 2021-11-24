#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

enum {
    POWERUP_FIRE, POWERUP_SKATE, POWERUP_BOMB, POWERUP_COUNT
};

class Powerup : public GameObject {
public:
    using GameObject::GameObject;

    inline static u32 id() { return 5; }

    Powerup(vec2i position);
    ~Powerup();
    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    void onHit();

    inline int getType() { return type; }
    u32 getTypeId() const override { return Powerup::id(); };

private:
    StaticBody collider;
    int type = 0;
};