#include "scene.h"

Scene *cur_scene = nullptr;

void swapScene(Scene *new_scene) {
    if(cur_scene) {
        cur_scene->onExit();
    }
    cur_scene = new_scene;
    if(cur_scene) {
        cur_scene->onEnter();
    }
}