#include <vector>

#include <raylib.h>
#include <tracelog.h>
#include <os.h>

#include <core/config.h>
#include <core/tilemap.h>
#include <core/assets.h>
#include <core/gui.h>

#include <utils/vec.h>
#include <utils/rect.h>
#include <utils/ptr.h>

#include <gameplay/map_scene.h>
#include <gameplay/menu_scene.h>

#include <net/net.h>

int main() {
    Config::load();

    if (!skInit()) {
        fatal("couldn't initialize sockets => %s", skGetErrorString());
    }

    vec2i resolution = Config::get().resolution;

    bool pixel_perfect = true;

    SetTraceLogCallback(traceLogVaList);

    InitWindow(resolution.x, resolution.y, "Client");
    SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(resolution.x, resolution.y);

    Font font = assets::getFont("res/HeartbitXX.png");
    GuiSetFont(font);
    GuiLoadStyle("res/candy.rgs");

    RenderTexture2D target = LoadRenderTexture(resolution.x, resolution.y);

    Texture2D bomb_texture = assets::getTexture("res/bomb.png");
    
    MenuScene menu;

    std::vector<Scene *> scenes;
    scenes.emplace_back(&menu);
    scenes.emplace_back(&gmap);

    for(auto scene : scenes) {
        scene->onInit();
    }

    //swapScene(&gjoin);
    swapScene(&menu);

    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        // == UPDATE =========================

        cur_scene->onUpdate();

        if(IsKeyPressed(KEY_P)) {
            pixel_perfect = !pixel_perfect;
        }

        // == RENDER =========================

        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            cur_scene->onRender();
        EndTextureMode();

        const vec2i src_sz { GetScreenWidth(), GetScreenHeight() };

        f32 scale = min(
            (f32)src_sz.x / resolution.x,
            (f32)src_sz.y / resolution.y
        );
        if(pixel_perfect) {
            scale = floor(scale);
        }
        vec2f scaled_res = resolution;
        scaled_res *= scale;

        Rectangle target_source {
            0, 0,
            (f32)target.texture.width,
            (f32)-target.texture.height
        };
        recti target_dest {
            (src_sz - scaled_res) / 2, scaled_res
        };

        SetMouseScale(1.f / scale, 1.f / scale);
        SetMouseOffset(-target_dest.x, -target_dest.y);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                target.texture,
                target_source, target_dest,
                { 0, 0 }, 0.f, WHITE
            );

            DrawFPS(10, 10);
        EndDrawing();
    }
    cur_scene->onExit();

    for(auto scene : scenes) {
        scene->onDestroy();
    }

    assets::destroyFont(font);
    assets::destroyTexture(bomb_texture);

    CloseWindow();
}