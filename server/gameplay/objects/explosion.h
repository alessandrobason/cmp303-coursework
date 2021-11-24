#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Explosion : public GameObject {
public:
    using GameObject::GameObject;

    inline static u32 id() { return 3; }

    Explosion(vec2i position, vec2i direction, int strength);
    ~Explosion();
    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    inline int getStrength() { return strength; }
    inline vec2i getDirection() { return direction; }
    inline StaticBody *getCollider() { return &collider; }
    inline u32 getTypeId() const override { return Explosion::id(); };

private:
    StaticBody collider;
    int strength = 1;
    vec2i direction;
    f32 timer = 0.1f;
};