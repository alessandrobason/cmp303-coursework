#include <raylib.h>
#include <tracelog.h>

#include <utils/vec.h>
#include <utils/rect.h>
#include <core/config.h>
#include <core/tilemap.h>

#include <utils/ptr.h>

#include <gameplay/map_scene.h>

int main() {
    Config::load();

    vec2i resolution = Config::get().resolution;

    bool pixel_perfect = true;

    SetTraceLogCallback(traceLogVaList);

    InitWindow(800, 800, "online game");
    SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(resolution.x, resolution.y);

    RenderTexture2D target = LoadRenderTexture(resolution.x, resolution.y);
    
    Scene *cur_scene = &gmap;

    SetTargetFPS(60);

    cur_scene->onInit();

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

        BeginDrawing();
            ClearBackground(BLACK);

            Rectangle target_source {
                0, 0,
                (f32)target.texture.width,
                (f32)-target.texture.height
            };
            recti target_dest {
                (src_sz - scaled_res) / 2, scaled_res
            };
            DrawTexturePro(
                target.texture,
                target_source, target_dest,
                { 0, 0 }, 0.f, WHITE
            );

            DrawFPS(10, 10);
        EndDrawing();
    }

    cur_scene->onExit();

    CloseWindow();
}