#include "menu_scene.h"

#include <core/gui.h>
#include <core/config.h>
#include <tracelog.h>

#include <gameplay/map_scene.h>

MenuScene::~MenuScene() {

}

void MenuScene::onInit() {
    panel_size.setSize(Config::get().resolution);

    panel_size.x = 48.f;
    panel_size.y = panel_size.h - 80;
    panel_size.w -= panel_size.x * 2.f;
    panel_size.h = 32.f;

    current = panel_size;

    padding = 24.f;
    f32 total_text_size = getTextSize("Join")    + getTextSize("Host") + 
                          getTextSize("Options") + getTextSize("Quit"); 
    spacing = (panel_size.w - total_text_size) / 3.f;
}

void MenuScene::onRender() {
    switch (current_state) {
    case State::main:    mainMenu();    break; 
    case State::join:    joinMenu();    break;
    case State::host:    hostMenu();    break;
    case State::options: optionsMenu(); break;
    }
}

void MenuScene::mainMenu() {
    current.setPosition(panel_size.position());

    if(button("Join")) {
        debug("Joining");
        current_state = State::join;
    }

    nextHor();

    if(button("Host")) {
        debug("Hosting");
        current_state = State::host;
    }

    nextHor();

    if(button("Options")) {
        debug("Optioning");
        current_state = State::options;
    }

    nextHor();

    if(button("Quit")) {
        debug("Quitting");
        AskCloseWindow();
    }
}

void MenuScene::joinMenu() {
    cur_scene = &gmap;
    onExit();
    cur_scene->onEnter();
}

void MenuScene::hostMenu() {

}

void MenuScene::optionsMenu() {

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