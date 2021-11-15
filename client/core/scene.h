#pragma once

class Scene {
public:
    virtual ~Scene() {};
    inline virtual void onInit() {};
    inline virtual void onEnter() {};
    inline virtual void onExit() {};
    inline virtual void onDestroy() {};
    inline virtual void onUpdate() {};
    inline virtual void onRender() {};
};

extern Scene *cur_scene;