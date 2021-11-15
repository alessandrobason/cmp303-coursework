#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Crate : public GameObject {
public:
    using GameObject::GameObject;

    Crate(vec2i position);
    ~Crate();
    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    void onHit();

private:
    StaticBody collider;
};
