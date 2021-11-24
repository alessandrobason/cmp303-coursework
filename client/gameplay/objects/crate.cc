#include "crate.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>
#include <gameplay/objects/powerup.h>

void Crate::onInit() {
    sprite.origin = { 0, 5 };
    sprite.load("res/crate.png");
}