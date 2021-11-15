#pragma once

#include <vector>

#include <utils/vec.h>
#include <core/sprite.h>
#include <core/id.h>

class GameObject {
public:
    GameObject();
    GameObject(GameObject &&other);

    virtual ~GameObject();
    virtual void onInit();
    virtual void onExit();
    virtual void onUpdate();
    virtual void onRender(bool is_debug = false);

    inline UniqueId getId() const { return unique_id; }
    inline TypeId getTypeId() const { return type_id; }
    inline vec2i getPosition() const { return sprite.position; }
    inline bool isDead() const { return dead; }
    inline void setDead(bool is_dead) { dead = is_dead; }

    bool operator<(const GameObject &other);

protected:
    TypeId type_id;
    UniqueId unique_id;
    Sprite sprite;
    bool dead = false;
};