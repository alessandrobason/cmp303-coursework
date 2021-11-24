#pragma once

#include <gameplay/game_object.h>
#include <gameplay/collider.h>

class Crate : public GameObject {
public:
    using GameObject::GameObject;

    inline static u32 id() { return 2; }

    Crate(vec2i position);
    ~Crate();
    void onInit() override;
    void onUpdate() override;
    void onRender(bool is_debug) override;

    void onHit();

    inline u32 getTypeId() const override { return Crate::id(); };

private:
    StaticBody collider;
};
