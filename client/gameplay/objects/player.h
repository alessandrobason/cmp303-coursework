#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Player : public GameObject {
public:
    using GameObject::GameObject;

    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    inline DynamicBody &getCollider() { return collider; }

private:
    void updateMovement();

    void playAnimation(u32 index);
    void setIdleAnim(vec2i dir);
    void setWalkingAnim(vec2i dir);
    int dirToInt(vec2i dir);

    i32 speed = 200;
    DynamicBody collider;

    bool is_locked = false;
    vec2i old_dir;
};