#if 0

#include "collider.h"

#include <assert.h>

b2World g_world(b2Vec2(0.f, 0.f));
constexpr f32 SCALE = 0.1f;

// STATIC_BODY --------------------------------------

StaticBody::StaticBody(const StaticBody &other)  {
    collision_response = other.collision_response;
    collision_udata = other.collision_udata;
    body = other.body;
    // when a vector resizes it may change the "this"
    // pointer, reset it just for good measure
    if(body)
        body->GetUserData().pointer = (uintptr_t)this;
}

void StaticBody::init(const vec2i &position, bool allow_sleep) {
    assert(body == nullptr && "init called after other functions");

    b2BodyDef def;
    def.fixedRotation = true;
    def.type = b2_staticBody;
    def.position.Set(position.x * SCALE, position.y * SCALE);
    def.userData.pointer = (uintptr_t)this;
    def.allowSleep = allow_sleep;
    
    body = g_world.CreateBody(&def);
}

void StaticBody::init(b2BodyDef &definition) {
    definition.type = b2_staticBody;
    definition.position *= SCALE;
    if (!definition.userData.pointer)
        definition.userData.pointer = (uintptr_t)this;

    body = g_world.CreateBody(&definition);
}

void StaticBody::cleanup() {
    if(body) {
        g_world.DestroyBody(body);
        body = nullptr;
    }
}

void StaticBody::setPosition(const vec2i &pos) {
    assert(body != nullptr && "SetPosition called before init");
    body->SetTransform({ (f32)pos.x * SCALE, (f32)pos.y * SCALE }, 0.f);
}

void StaticBody::setFixture(
    const b2Vec2 *points, 
    int32 count, 
    float friction, 
    float restitution, 
    float density, 
    physics_layers layer, 
    physics_layers mask, 
    bool is_sensor
) {
    b2PolygonShape shape;
    shape.Set(points, count);

    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = density;
    fixture.friction = friction;
    fixture.filter.categoryBits = layer;
    fixture.filter.maskBits = mask;

    setFixture(fixture);
}

void StaticBody::setFixture(
    const std::vector<b2Vec2> &points, 
    float friction, 
    float restitution, 
    float density, 
    physics_layers layer, 
    physics_layers mask, 
    bool is_sensor
) {
    setFixture(points.data(), (int32)points.size(), friction, restitution, density, layer, mask, is_sensor);
}

void StaticBody::setFixture(const b2FixtureDef &fixture) {
    assert(body != nullptr && "SetFixture called before init");
    body->CreateFixture(&fixture);
}

void StaticBody::setFixtureAsBox(
    const vec2i &scale, 
    float friction, 
    float restitution, 
    float density, 
    const vec2i &center, 
    float angle, 
    physics_layers layer, 
    physics_layers mask, 
    bool is_sensor
) {
    b2PolygonShape shape;
    shape.SetAsBox(
        f32(scale.x / 2) * SCALE, 
        f32(scale.y / 2) * SCALE, 
        { (f32)center.x * SCALE, (f32)center.y * SCALE }, 
        angle
    );

    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = density;
    fixture.friction = friction;
    fixture.filter.categoryBits = layer;
    fixture.filter.maskBits = mask;
    fixture.restitution = restitution;
    fixture.isSensor = is_sensor;

    setFixture(fixture);
}

void StaticBody::setFixtureAsBox(
    const vec2i &scale, 
    float friction, 
    float restitution, 
    float density, 
    physics_layers layer, 
    physics_layers mask, 
    bool is_sensor
) {
    setFixtureAsBox(scale, friction, restitution, density, scale / 2, 0.f, layer, mask, is_sensor);
}

void StaticBody::setFixtureAsBox(
    const vec2i &scale, 
    physics_layers layer, 
    physics_layers mask
) {
    setFixtureAsBox(scale, 0.2f, 0.f, 0.f, scale / 2, 0.f, layer, mask, false);
}

void StaticBody::setFixtureAsBox(
    const vec2i &scale, 
    const vec2i &center, 
    float angle, 
    physics_layers layer, 
    physics_layers mask
) {
    setFixtureAsBox(scale, 0.2f, 0.f, 0.f, center, angle, layer, mask, false);
}

void StaticBody::setFixtureAsCircle(
    const vec2i &pos, 
    i32 radius, 
    float friction, 
    float restitution, 
    float density, 
    physics_layers layer, 
    physics_layers mask,
    bool is_sensor
) {
    b2CircleShape shape;
    shape.m_p = { (f32)pos.x * SCALE, (f32)pos.y * SCALE };
    shape.m_radius = (f32)radius * SCALE;

    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = density;
    fixture.friction = friction;
    fixture.filter.categoryBits = layer;
    fixture.filter.maskBits = mask;
    fixture.restitution = restitution;
    fixture.isSensor = is_sensor;

    setFixture(fixture);
}

void StaticBody::setFixtureAsCircle(
    const vec2i &pos, 
    i32 radius, 
    physics_layers layer,
    physics_layers mask
) {
    setFixtureAsCircle(pos, radius, 0.2f, 0.f, 0.f, layer, mask, false);
}

vec2i StaticBody::getPosition() {
    b2Vec2 pos = body->GetPosition();
    return {
        (i32)roundf(pos.x / SCALE),
        (i32)roundf(pos.y / SCALE)
    };
}

#include <tracelog.h>

void StaticBody::render(Color color) {
    if(!body || !body->IsEnabled()) return;

    Vector2 pos;
    {
        auto &temp = body->GetPosition();
        pos.x = temp.x / SCALE;
        pos.y = temp.y / SCALE;
    }
    b2Fixture *fix = body->GetFixtureList();

    Color bg = color;
    bg.a = 100;

    while(fix) {
        switch(fix->GetType()) {
            case b2Shape::e_circle: {
                b2CircleShape *circle = (b2CircleShape *)fix->GetShape();
                int x = int(round(circle->m_p.x / SCALE) + pos.x);
                int y = int(round(circle->m_p.y / SCALE) + pos.y);
                DrawCircle(x, y, circle->m_radius / SCALE, bg);
                DrawCircleLines(x, y, circle->m_radius / SCALE, color);
                break;
            }
            case b2Shape::e_polygon: {
                b2PolygonShape *poly = (b2PolygonShape *)fix->GetShape();
                auto &verts = poly->m_vertices;
                
                int x = (int)round(verts[0].x / SCALE + pos.x);
                int y = (int)round(verts[0].y / SCALE + pos.y);
                int w = (int)round(verts[1].x / SCALE - verts[0].x / SCALE);
                int h = (int)round(verts[2].y / SCALE - verts[0].y / SCALE);

                DrawRectangle(x, y, w, h, bg);
                DrawRectangleLines(x, y, w, h, color);
                break;
            }
        }
        fix = fix->GetNext();
    }
}

u16 StaticBody::getMask() {
    return body->GetFixtureList()->GetFilterData().categoryBits;
}

// KINEMATIC_BODY --------------------------------------

void KinematicBody::init(const vec2i &position, bool allow_sleep, float angle, bool is_bullet) {
    b2BodyDef def;
    def.type = b2_kinematicBody;
    def.position.Set(position.x * SCALE, position.y * SCALE);
    def.userData.pointer = (uintptr_t)this;
    def.angle = angle;
    def.bullet = is_bullet;
    def.allowSleep = allow_sleep;

    body = g_world.CreateBody(&def);
}

void KinematicBody::init(b2BodyDef &definition) {
    definition.type = b2_kinematicBody;
    definition.position *= SCALE;
    if (!definition.userData.pointer)
        definition.userData.pointer = (uintptr_t)this;

    body = g_world.CreateBody(&definition);
}

// DYNAMIC_BODY --------------------------------------

void DynamicBody::init(const vec2i &position, bool allow_sleep, float angle, bool is_bullet) {
    b2BodyDef def;
    def.fixedRotation = true;
    def.type = b2_dynamicBody;
    def.position.Set(position.x * SCALE, position.y * SCALE);
    def.userData.pointer = (uintptr_t)this;
    def.angle = angle;
    def.bullet = is_bullet;
    def.allowSleep = allow_sleep;

    body = g_world.CreateBody(&def);
}

void DynamicBody::init(b2BodyDef &definition) {
    definition.type = b2_dynamicBody;
    definition.position *= SCALE;
    if(!definition.userData.pointer)
        definition.userData.pointer = (uintptr_t)this;

    body = g_world.CreateBody(&definition);
}

#endif