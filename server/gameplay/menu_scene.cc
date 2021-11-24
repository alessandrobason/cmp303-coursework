#include "menu_scene.h"

#include <core/gui.h>
#include <core/config.h>
#include <tracelog.h>

#include <gameplay/map_scene.h>
#include <gameplay/host_scene.h>
#include <gameplay/join_scene.h>

MenuScene::~MenuScene() {

}

void MenuScene::onInit() {
    panel_size.setSize(Config::get().resolution);

    panel_size.x = 100.f;
    panel_size.y = panel_size.h / 2;
    panel_size.w -= panel_size.x * 2.f;
    panel_size.h = 32.f;

    current = panel_size;

    padding = 24.f;
    f32 total_text_size = getTextSize("2") + getTextSize("3") + getTextSize("4");
    spacing = (panel_size.w - total_text_size) / 2.f;
}

void MenuScene::onRender() {
    current.setPosition(panel_size.position());

    GuiLabel({80, 130, 0, 0}, "Choose how many players");

    if (button("2")) {
        ghost.setPlayerNumber(2);
        swapScene(&ghost);
    }

    nextHor();

    if (button("3")) {
        ghost.setPlayerNumber(3);
        swapScene(&ghost);
    }

    nextHor();

    if (button("4")) {
        ghost.setPlayerNumber(4);
        swapScene(&ghost);
    }
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