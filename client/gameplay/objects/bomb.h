#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Bomb : public GameObject {
public:
    using GameObject::GameObject;

    Bomb(vec2i position, u16 source_layer, int strength);
    ~Bomb();
    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    inline void setIsInside(bool inside) { is_inside = inside; }
    inline StaticBody &getCollider() { return collider; }

private:
    void makeSolid();
    void increasePlayerBombs();

    StaticBody collider;
    u16 mask = ALL_LAYERS;
    bool is_inside = true;
    bool is_solid = false;
    int strength = 0;
    f32 timer = 3.f;
};