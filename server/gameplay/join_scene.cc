#include "join_scene.h"

#include <net/net.h>
#include <gameplay/map_scene.h>

JoinScene gjoin;

JoinScene::~JoinScene() {

}

void JoinScene::onEnter() {
    net.init(false);
}

void JoinScene::onExit() {
}

void JoinScene::onUpdate() {
    if(net.isReady()) {
        info("READY!");
        swapScene(&gmap);
    }
    else {
        info("not ready");
    }
}

void JoinScene::onRender() {

}
