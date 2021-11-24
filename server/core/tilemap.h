#pragma once

#include <string>
#include <raylib.h>
#include <utils/types.h>
#include <utils/dynarray.h>
#include <utils/vec.h>
#include <utils/rect.h>
#include <gameplay/collider.h>

struct TileAnimation {
    f32 timer = 0.f;
    f32 duration;
    u32 cur_frame = 0;
    dynarray<i32> animation;
    dynarray<u32> tiles;
};

struct Tileset {
    Texture2D texture;
    vec2i size;
};

struct Tilemap {
    Tileset tileset;
    vec2i position;
    vec2i size;
    i32 layer_count;
    i32 layers_under = 0;
    dynarray<i32> tiles;
    dynarray<TileAnimation> animations;
    StaticBody colliders;

    ~Tilemap();
    void onExit();

    void loadFromJSON(const std::string &fname);

    vec2i mapSizePixels() const;

    recti getBounds() const;
    i32 positionToIndex(const vec2i &position) const;

    void update();

    Tilemap &operator=(Tilemap &&other);
};

void drawTilemapUnder(Tilemap &tm);
void drawTilemapOver(Tilemap &tm);
void drawTilemapLayer(Tilemap &tm, i32 layer);
void drawTilemap(Tilemap &tm);