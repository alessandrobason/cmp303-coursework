#include "bomb.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>

void Bomb::onInit() {
    sprite.load("res/bomb.png", 1, 4, 0.2f);
}