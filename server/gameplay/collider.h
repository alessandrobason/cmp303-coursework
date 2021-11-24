#pragma once

#include <vector>

#include <box2d/box2d.h>
#include <utils/vec.h>

extern b2World g_world;

enum physics_layers : u16 {
	NO_LAYER = 1 << 0, 
	LAYER_1  = 1 << 1, // PLAYER 1 
	LAYER_2  = 1 << 2, // PLAYER 2 
	LAYER_3  = 1 << 3, // PLAYER 3 
	LAYER_4  = 1 << 4, // PLAYER 4 
	LAYER_5  = 1 << 5, // BOMBS
	LAYER_6  = 1 << 6,
	LAYER_7  = 1 << 7, // CRATES
	LAYER_8  = 1 << 8,
	LAYER_9  = 1 << 9,
    ALL_LAYERS = 0xffff
};

typedef void (*CollisionResponseFunc)(b2Body *, void *);

class StaticBody {
public:
    StaticBody() = default;
    StaticBody(const StaticBody &other);

    void init(const vec2i &position = vec2i::zero(), bool allow_sleep = true);
    void init(b2BodyDef &definition);

    void cleanup();
    void setPosition(const vec2i &position);

    void setFixture(const b2Vec2 *points, int32 count, float friction, float restitution, float density, physics_layers layer = NO_LAYER, physics_layers mask = ALL_LAYERS, bool is_sensor = false);
    void setFixture(const std::vector<b2Vec2> &points, float friction, float restitution, float density, physics_layers layer = NO_LAYER, physics_layers mask = ALL_LAYERS, bool is_sensor = false);
    void setFixture(const b2FixtureDef &fixture);
    
    void setFixtureAsBox(const vec2i &scale, float friction, float restitution, float density, const vec2i &center, float angle = 0.f, physics_layers layer = NO_LAYER, physics_layers mask = ALL_LAYERS, bool is_sensor = false);
    void setFixtureAsBox(const vec2i &scale, float friction, float restitution, float density, physics_layers layer = NO_LAYER, physics_layers mask = ALL_LAYERS, bool is_sensor = false);
    void setFixtureAsBox(const vec2i &scale, physics_layers layer = NO_LAYER, physics_layers mask = ALL_LAYERS);
    void setFixtureAsBox(const vec2i &scale, const vec2i &center, float angle = 0.f, physics_layers layer = NO_LAYER, physics_layers mask = ALL_LAYERS);

    void setFixtureAsCircle(const vec2i &pos, i32 radius, float friction, float restitution, float density, physics_layers layer = NO_LAYER, physics_layers mask = ALL_LAYERS, bool is_sensor = false);
    void setFixtureAsCircle(const vec2i &pos, i32 radius, physics_layers = NO_LAYER, physics_layers mask = ALL_LAYERS);

    vec2i getPosition();

    void render(Color color);

    u16 getMask();

    inline void setCollisionResponse(CollisionResponseFunc function);
    // gets called when a collision happens
    inline void collision(b2Body *other);

    inline void *getUserData();
    inline void setUserData(void *data);

    inline b2Body *getBody();
    inline void setBody(b2Body *new_body);

protected:
    b2Body *body = nullptr;
    CollisionResponseFunc collision_response = nullptr;
    void *collision_udata = nullptr;
};

class KinematicBody : public StaticBody {
public:
    KinematicBody() = default;

    void init(const vec2i &position = vec2i::zero(), bool allow_sleep = true, float angle = 0.f, bool is_bullet = false);
    void init(b2BodyDef &definition);
};

class DynamicBody : public StaticBody {
public:
    DynamicBody() = default;

    void init(const vec2i &position = vec2i::zero(), bool allow_sleep = true, float angle = 0.f, bool is_bullet = false);
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
