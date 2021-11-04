#include "game_object.h"

#include <string.h>

#include <utils/math_utils.h>
#include <core/assets.h>

u32 count = 0;

GameObject::GameObject(GameObject &&other) {
    unique_id = other.unique_id;
    sprite = std::move(other.sprite);
    other.unique_id = 0;
}

GameObject::~GameObject() {
}

void GameObject::onInit() {
}

void GameObject::onExit() {
    assets::destroyTexture(sprite.texture);
}

void GameObject::onUpdate() {
    assert(unique_id != 0 && "didn't initialize id");
    sprite.update();
}

void GameObject::onRender(bool is_debug) {
    drawSprite(sprite);
}

bool GameObject::operator<(const GameObject &other) {
    return sprite.position.y < other.sprite.position.y;
}

