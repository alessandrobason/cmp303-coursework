#pragma once

class Scene {
public:
    virtual ~Scene() {};
    virtual void onInit() = 0;
    virtual void onExit() = 0;
    virtual void onUpdate() = 0;
    virtual void onRender() = 0;
};