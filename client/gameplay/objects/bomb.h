#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Bomb : public GameObject {
public:
    using GameObject::GameObject;

    constexpr static u32 id(){ return 1; }

    void onInit() override;

    inline u32 getTypeId() const override { return Bomb::id(); };
};
