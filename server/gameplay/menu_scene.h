#pragma once

#include <core/scene.h>
#include <utils/rect.h>

class MenuScene : public Scene {
public:
    ~MenuScene() override;
    void onInit() override;
    void onRender() override;

private:
    void nextHor();
    void nextVer();
    bool button(const char *text);
    f32 getTextSize(const char *text);

    rectf panel_size;
    rectf current;
    f32 spacing;
    f32 padding;
};