#include "collider.h"

#include <assert.h>

b2World g_world(b2Vec2(0.f, 0.f));

// STATIC_BODY --------------------------------------
	
void StaticBody::init(const vec2i &position) {
    assert(body == nullptr && "init called after other functions");

    b2BodyDef def;
    def.fixedRotation = true;
    def.type = b2_staticBody;
    def.position.Set(position.x, position.y);
    def.userData.pointer = (uintptr_t)this;
    body = g_world.CreateBody(&def);
}

void StaticBody::init(b2BodyDef &definition) {
    definition.type = b2_staticBody;
    if (!definition.userData.pointer)
        definition.userData.pointer = (uintptr_t)this;

    body = g_world.CreateBody(&definition);
}

void StaticBody::cleanup() {
    g_world.DestroyBody(body);
}

void StaticBody::setPosition(const vec2i &pos) {
    assert(body != nullptr && "SetPosition called before init");

    body->SetTransform({ (f32)pos.x, (f32)pos.y }, 0.f);
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
        f32(scale.x / 2), 
        f32(scale.y / 2), 
        { (f32)center.x, (f32)center.y }, 
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
    shape.m_p = { (f32)pos.x, (f32)pos.y };
    shape.m_radius = (f32)radius;

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

#include <tracelog.h>

void StaticBody::render(Color color) {
    if(!body) return;

    Vector2 pos;
    {
        auto &temp = body->GetPosition();
        pos.x = temp.x;
        pos.y = temp.y;
    }
    b2Fixture *fix = body->GetFixtureList();

    Color bg = color;
    bg.a = 100;

    while(fix) {
        switch(fix->GetType()) {
            case b2Shape::e_circle: {
                b2CircleShape *circle = (b2CircleShape *)fix->GetShape();
                int x = (int)round(circle->m_p.x) + pos.x;
                int y = (int)round(circle->m_p.y) + pos.y;
                DrawCircle(x, y, circle->m_radius, bg);
                DrawCircleLines(x, y, circle->m_radius, color);
                break;
            }
            case b2Shape::e_polygon: {
                b2PolygonShape *poly = (b2PolygonShape *)fix->GetShape();
                auto &verts = poly->m_vertices;
                
                int x = (int)round(verts[0].x + pos.x);
                int y = (int)round(verts[0].y + pos.y);
                int w = (int)round(verts[1].x - verts[0].x);
                int h = (int)round(verts[2].y - verts[0].y);

                DrawRectangle(x, y, w, h, bg);
                DrawRectangleLines(x, y, w, h, color);
                break;
            }
        }
        fix = fix->GetNext();
    }
}

// KINEMATIC_BODY --------------------------------------

void KinematicBody::init(const vec2i &position, float angle, bool is_bullet) {
    b2BodyDef def;
    def.type = b2_kinematicBody;
    def.position.Set(position.x, position.y);
    def.userData.pointer = (uintptr_t)this;
    def.angle = angle;
    def.bullet = is_bullet;

    body = g_world.CreateBody(&def);
}

void KinematicBody::init(b2BodyDef &definition) {
    definition.type = b2_kinematicBody;
    if (!definition.userData.pointer)
        definition.userData.pointer = (uintptr_t)this;

    body = g_world.CreateBody(&definition);
}

// DYNAMIC_BODY --------------------------------------

void DynamicBody::init(const vec2i &position, float angle, bool is_bullet) {
    b2BodyDef def;
    def.fixedRotation = true;
    def.type = b2_dynamicBody;
    def.position.Set(position.x, position.y);
    def.userData.pointer = (uintptr_t)this;
    def.angle = angle;
    def.bullet = is_bullet;

    body = g_world.CreateBody(&def);
}

void DynamicBody::init(b2BodyDef &definition) {
    definition.type = b2_dynamicBody;
    if(!definition.userData.pointer)
        definition.userData.pointer = (uintptr_t)this;

    body = g_world.CreateBody(&definition);
}