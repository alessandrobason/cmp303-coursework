#include "game_object.h"

#include <string.h>

#include <utils/math_utils.h>
#include <core/assets.h>

GameObject::GameObject() {
    unique_id = getUniqueId();
}

GameObject::GameObject(GameObject &&other) {
    unique_id = other.unique_id;
    type_id = other.type_id;
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
    assert(type_id.id != 0 && unique_id.id != 0 && "didn't initialize ids");
    sprite.update();
}

void GameObject::onRender(bool is_debug) {
    drawSprite(sprite);
}

bool GameObject::operator<(const GameObject &other) {
    return sprite.position.y < other.sprite.position.y;
}

