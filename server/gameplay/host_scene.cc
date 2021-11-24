#include "host_scene.h"

#include <http.h>

#include <core/gui.h>
#include <gameplay/objects/player.h>
#include <gameplay/map_scene.h>
#include <net/net.h>
#include <utils/base26.h>

HostScene ghost;

HostScene::~HostScene() {

}

void HostScene::onEnter() {
    ip = net.getGlobalIp();
}

void HostScene::onExit() {
}

void HostScene::onUpdate() {
    if (net.conn_count == (player_num + 1)) {
        info("READY!");

        for (auto &crate : gmap.getCrates()) {
            Message<AddRemMsg> msg{
                MSG_OBJ_ADD,
                { (u8)Crate::id(), crate }
            };
            net.sendMessage(msg, SEND_ALL);
        }

        int pcount = net.conn_count - 1;
        for (int i = 0; i < pcount; ++i) {
            players[i].addPlayer(i, true);

            Message<PlayerName> msg;
            msg.type = MSG_PLAYER_NAME;
            msg.data.id = (u8)i;
            memcpy(msg.data.name, players[i].getName(), sizeof(msg.data.name));
            
            net.sendMessage(msg, SEND_ALL);
        }

        for (int i = 1; i < net.conn_count; ++i) {
            for (int j = 1; j < net.conn_count; ++j) {
                Message<PlayerNew> msg{
                    MSG_NEW_PLAYER, { u8(j - 1), j == i }
                };
                net.sendMessage(msg, i);
            }
        }

        Message<u8> msg{ MSG_READY, (u8)gmap.getMapId()};
        net.sendMessage(msg, SEND_ALL);

        swapScene(&gmap);
    }
}

void HostScene::onRender() {
    GuiPanel({ 0, 0, 400, 336 });
    GuiLabel({ 50, 50, 200, 36 }, TextFormat("local ip: %s\nglobal ip: %s", net.ip.c_str(), ip.c_str()));
}
