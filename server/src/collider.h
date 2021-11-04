#pragma once

#include <vector>

#include <box2d/box2d.h>
#include <utils/vec.h>

extern b2World g_world;

enum physics_layers {
	NO_LAYER = 1 << 0, // ground
	LAYER_1  = 1 << 1, // fruits
	LAYER_2  = 1 << 2, // player
	LAYER_3  = 1 << 3, // water, juice maker
	LAYER_4  = 1 << 4, // juice
	LAYER_5  = 1 << 5,
	LAYER_6  = 1 << 6,
	LAYER_7  = 1 << 7,
	LAYER_8  = 1 << 8,
	LAYER_9  = 1 << 9
};

typedef void (*CollisionResponseFunc)(b2Body *, void *);

class StaticBody {
public:
    StaticBody() = default;
    StaticBody(const StaticBody &other) {
        collision_response = other.collision_response;
        collision_udata = other.collision_udata;
        body = other.body;
        // when a vector resizes it may change the "this"
        // pointer, reset it just for good measure
        if(body)
            body->GetUserData().pointer = (uintptr_t)this;
    }
    ~StaticBody() = default;

    void init(const vec2i &position = vec2i::zero());
    void init(b2BodyDef &definition);

    void cleanup();
    void setPosition(const vec2i &position);

    void setFixture(const b2Vec2 *points, int32 count, float friction, float restitution, float density, physics_layers layer = NO_LAYER, bool is_sensor = false);
    void setFixture(const std::vector<b2Vec2> &points, float friction, float restitution, float density, physics_layers layer = NO_LAYER, bool is_sensor = false);
    void setFixture(const b2FixtureDef &fixture);
    
    void setFixtureAsBox(const vec2i &scale, float friction, float restitution, float density, const vec2i &center, float angle = 0.f, physics_layers layer = NO_LAYER, bool is_sensor = false);
    void setFixtureAsBox(const vec2i &scale, float friction, float restitution, float density, physics_layers layer = NO_LAYER, bool is_sensor = false);
    void setFixtureAsBox(const vec2i &scale, physics_layers layer = NO_LAYER);
    void setFixtureAsBox(const vec2i &scale, const vec2i &center, float angle = 0.f, physics_layers layer = NO_LAYER);

    void setFixtureAsCircle(const vec2i &pos, i32 radius, float friction, float restitution, float density, physics_layers layer = NO_LAYER, bool is_sensor = false);
    void setFixtureAsCircle(const vec2i &pos, i32 radius, physics_layers = NO_LAYER);

    inline void setCollisionResponse(CollisionResponseFunc function);
    // gets called when a collision happens
    inline void collision(b2Body *other);

    inline void *getUserData();
    inline void setUserData(void *data);

    inline b2Body *getBody();
    inline void setBody(b2Body *new_body);

    inline vec2i getPosition();

protected:
    b2Body *body = nullptr;
    CollisionResponseFunc collision_response = nullptr;
    void *collision_udata = nullptr;
};

class KinematicBody : public StaticBody {
public:
    KinematicBody() = default;

    void init(const vec2i &position = vec2i::zero(), float angle = 0.f, bool is_bullet = false);
    void init(b2BodyDef &definition);
};

class DynamicBody : public StaticBody {
public:
    DynamicBody() = default;

    void init(const vec2i &position = vec2i::zero(), float angle = 0.f, bool is_bullet = false);
    void init(b2BodyDef &definition);
};

/* == INLINE FUNCTIONS =========================== */

// STATIC_BODY --------------------------------------

inline void StaticBody::setCollisionResponse(CollisionResponseFunc function) {
    collision_response = function;
}

inline void StaticBody::collision(b2Body *other) {
    if (collision_response != nullptr)
        collision_response(other, collision_udata);
}

inline void *StaticBody::getUserData() {
    return collision_udata;
}

inline void StaticBody::setUserData(void *data) {
    collision_udata = data;
}

b2Body *StaticBody::getBody() {
    return body;
}

void StaticBody::setBody(b2Body *new_body) {
    assert(body != new_body);
    body = new_body;
}

vec2i StaticBody::getPosition() {
    b2Vec2 p = body->GetPosition();
    return { (i32)p.x, (i32)p.y };
}