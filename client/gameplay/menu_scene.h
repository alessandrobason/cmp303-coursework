#pragma once

#include <core/scene.h>
#include <core/sprite.h>
#include <utils/rect.h>

class MenuScene : public Scene {
    enum class State {
        main, join, play
    };

public:
    ~MenuScene() override;
    void onInit() override;
    void onExit() override;
    void onRender() override;

private:
    void mainMenu(); 
    void playMenu();
    void joinMenu();

    void drawBackground();

    void nextHor();
    void nextVer();
    bool button(const char *text);
    f32 getTextSize(const char *text);

    State current_state = State::main;

    rectf panel_size;
    rectf current;
    f32 spacing;
    f32 padding;

    Sprite bomb;
    Sprite bg;

    char ip_buf[80];
};