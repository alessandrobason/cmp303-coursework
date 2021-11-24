#include "game_object.h"

#include <string.h>

#include <utils/math_utils.h>
#include <core/assets.h>

GameObject::GameObject() {
    unique_id = getUniqueId();
}

GameObject::GameObject(vec2i position) {
    sprite.position = position;
}

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
    sprite.update();
}

void GameObject::onRender(bool is_debug) {
    drawSprite(sprite);
}

bool GameObject::operator<(const GameObject &other) const {
    return sprite.position.y < other.sprite.position.y;
}

