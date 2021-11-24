#pragma once

#include <core/scene.h>

class JoinScene : public Scene {
public:
    ~JoinScene() override;
    void onEnter() override;
    void onExit() override;
    void onUpdate() override;
    void onRender() override;

private:

};

extern JoinScene gjoin;