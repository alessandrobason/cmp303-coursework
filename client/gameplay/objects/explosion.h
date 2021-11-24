#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Explosion : public GameObject {
public:
    using GameObject::GameObject;

    constexpr static u32 id() { return 3; }

    void onInit() override;
    void onUpdate() override;
    inline u32 getTypeId() const override { return Explosion::id(); };
};