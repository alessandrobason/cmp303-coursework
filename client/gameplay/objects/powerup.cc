#include "powerup.h"

#include <tracelog.h>
#include <gameplay/map_scene.h>

void Powerup::onInit() {
    debug(">> type = %d", type);
    sprite.load("res/powerups.png", {16 * type, 0, 16, 16});
}