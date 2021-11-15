#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Explosion : public GameObject {
public:
    using GameObject::GameObject;

    Explosion(vec2i position, vec2i direction, int strength);
    ~Explosion();
    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    inline int getStrength() { return strength; }
    inline vec2i getDirection() { return direction; }
    inline StaticBody *getCollider() { return &collider; }

private:
    StaticBody collider;
    int strength = 1;
    vec2i direction;
    f32 timer = 0.1f;
};