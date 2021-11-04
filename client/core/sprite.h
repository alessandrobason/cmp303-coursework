#pragma once
#include <raylib.h>

#include <string>

#include <utils/types.h>
#include <utils/vec.h>
#include <utils/rect.h>
#include <utils/dynarray.h>

struct Sprite;
using AnimFinishedCB = void (*)(Sprite&);

struct Animation {
    f32 timer = 0.f;
    f32 duration;
    u32 cur_image = 0;
    bool repeat = true;
    dynarray<u32> img_ids;

    bool finished();
};

struct Sprite {
    vec2i position;
    vec2i size;
    vec2i origin = vec2i::zero();
    vec2f scale = vec2f::one();
    f32 rotation = 0.f;
    Texture2D texture;
    u32 cur_animation = 0;
    dynarray<vec2f> anim_positions;
    dynarray<Animation> animations;

    Sprite();
    Sprite(Sprite &&spr);
    ~Sprite();

    Sprite &operator=(Sprite &&spr);

    void load(const std::string &fname);
    void load(const std::string &fname, i32 rows, i32 columns, f32 frame_duration);

    void loadFromTexture(const std::string &fname);
    void loadFromSheet(const std::string &fname, i32 rows, i32 columns, f32 frame_duration);
    void loadFromJSON(const std::string &fname);
    void loadNPC(u32 id);

    void play(u32 animation_id);

    void update();
    void updateCallback(AnimFinishedCB callback);
};

void drawSprite(const Sprite &spr);