#pragma once

#include <core/scene.h>
#include <utils/rect.h>

class MenuScene : public Scene {
    enum class State {
        main, join, host, options
    };

public:
    ~MenuScene() override;
    void onInit() override;
    void onRender() override;

private:
    void mainMenu(); 
    void joinMenu();
    void hostMenu();
    void optionsMenu();

    void nextHor();
    void nextVer();
    bool button(const char *text);
    f32 getTextSize(const char *text);

    State current_state = State::main;

    rectf panel_size;
    rectf current;
    f32 spacing;
    f32 padding;
};