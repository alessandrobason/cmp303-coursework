#pragma once

#include <string>
#include <core/scene.h>

class HostScene : public Scene {
public:
    ~HostScene() override;
    void onEnter() override;
    void onExit() override;
    void onUpdate() override;
    void onRender() override;

    inline void setPlayerNumber(int n) { player_num = n; }

private:
    std::string ip = "localhost";
    int player_num = 4;
};

extern HostScene ghost;