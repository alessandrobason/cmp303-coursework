#pragma once

#include <vector>

#include <utils/vec.h>
#include <core/sprite.h>

extern u32 count;

template<typename T>
u32 getUniqueId(T *ptr = nullptr) {
    static u32 id = ++count;
    return id;
}

class GameObject {
public:
    GameObject() = default;
    GameObject(GameObject &&other);

    virtual ~GameObject();
    virtual void onInit();
    virtual void onExit();
    virtual void onUpdate();
    virtual void onRender(bool is_debug = false);

    inline u32 getId() const { return unique_id; }
    inline vec2i getPosition() const { return sprite.position; }

    bool operator<(const GameObject &other);

protected:
    u32 unique_id = 0;
    Sprite sprite;
};