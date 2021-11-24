#include "menu_scene.h"

#include <core/assets.h>
#include <core/gui.h>
#include <core/config.h>
#include <tracelog.h>

#include <gameplay/map_scene.h>

#include <utils/base26.h>

#include <net/net.h>

MenuScene::~MenuScene() {
}

void MenuScene::onInit() {
    panel_size.setSize(Config::get().resolution);

    panel_size.x = 110.f;
    panel_size.y = panel_size.h - 60;
    panel_size.w -= panel_size.x * 2.f;
    panel_size.h = 32.f;

    current = panel_size;

    padding = 24.f;
    f32 total_text_size = getTextSize("Play") + getTextSize("Quit"); 
    spacing = panel_size.w - total_text_size;

    bomb.load("res/bomb.png", 1, 4, 0.2f);
    bg.load("res/bg.png");

    std::string local_ip = net.getLocalIp();
    if (!local_ip.empty()) {
        strcpy(ip_buf, local_ip.c_str());
    }
    else {
        strcpy(ip_buf, "127.0.0.1");
    }
}

void MenuScene::onExit() {
}

void MenuScene::onRender() {
    drawBackground();

    switch (current_state) {
    case State::main:    mainMenu();    break; 
    case State::join:    joinMenu();    break;
    case State::play:    playMenu();    break;
    }
}

void MenuScene::mainMenu() {
    current.setPosition(panel_size.position());

    static vec2f res = Config::get().resolution;
    static vec2f pos = vec2f::zero();
    
    if(button("Play")) {
        debug("Playing");
        current_state = State::play;
    }

    nextHor();

    if(button("Quit")) {
        debug("Quitting");
        AskCloseWindow();
    }
}

void MenuScene::playMenu() {
    GuiTextBox({ 125, 132, 150, 32 }, ip_buf, sizeof(ip_buf), true);
    if (GuiButton({125, 172, 150, 32}, "Play")) {
        net.ip = ip_buf;
        info("ip : %s", net.ip.c_str());
        memset(ip_buf, 0, sizeof(ip_buf));
        net.init(false);
        current_state = State::join;
    }
}

void MenuScene::joinMenu() {
    if (gmap.isReady()) {
        info("READY!");
        for (int i = 0; i < player_count; ++i) {
            debug("player %d name: %s", i, players[i].getName());
        }
        swapScene(&gmap);
    }

    static int count = 0;
    count = (count + 1) % 150;
    const char *text;
    f32 x = 0;

    if (net.isReady()) {
        GuiPanel({ 125, 132, 150, 50 });
        if (count < 50)       { text = "Loading.";   x = 164; }
        else if (count < 100) { text = "Loading..";  x = 162; }
        else if (count < 150) { text = "Loading..."; x = 160; }
        GuiLabel({ x, 140, 0, 32 }, text);
    }
    else {
        GuiPanel({ 80, 132, 240, 50 });
        if (count < 50)       { text = "Connecting to server.";   x = 103; }
        else if (count < 100) { text = "Connecting to server..";  x = 101; }
        else if (count < 150) { text = "Connecting to server..."; x = 99; }
        GuiLabel({ x, 140, 0, 32 }, text);
    }
}

void MenuScene::drawBackground() {
    static int w = Config::get().resolution.x;
    static int h = Config::get().resolution.y;
    static Color c = { 0, 0, 0, 100 };

    drawSprite(bg);
    //DrawRectangle(0, 0, w, h, c);
}


void MenuScene::nextHor() {
    current.x += (current.w) + spacing;
}

void MenuScene::nextVer() {
    current.y += (current.h / 2.f) + spacing;
}

bool MenuScene::button(const char *text) {
    current.w = getTextSize(text);
    return GuiButton(current, text);
}

struct TextData {
    Font font;
    int size;
    int spacing;

    TextData() {
        font = GuiGetFont();
        size = GuiGetStyle(DEFAULT, TEXT_SIZE);
        spacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
    }

    inline f32 getSize(const char *text) {
        return MeasureTextEx(font, text, size, spacing).x;
    }
};

f32 MenuScene::getTextSize(const char *text) {
    static TextData tdata;
    return tdata.getSize(text) + padding;
}