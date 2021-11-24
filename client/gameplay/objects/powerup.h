#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

enum {
    POWERUP_FIRE, POWERUP_SKATE, POWERUP_BOMB, POWERUP_COUNT
};

class Powerup : public GameObject {
public:
    using GameObject::GameObject;

    constexpr static u32 id() { return 5; }

    void onInit() override;

    inline void setType(int newtype) { type = newtype; }
    inline int getType() { return type; }
    u32 getTypeId() const override { return Powerup::id(); };

private:
    int type = 0;
};