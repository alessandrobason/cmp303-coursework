#include "sprite.h"

#include <math.h>
#include <stdio.h>
#include <core/json.h>
#include <core/assets.h>

#include <tracelog.h>

bool Animation::finished() {
    return cur_image >= img_ids.size();
}

Sprite::Sprite() {
    texture.id = 0;
}

Sprite::Sprite(Sprite &&spr) {
    *this = std::move(spr);
}

Sprite::~Sprite() {
    assets::destroyTexture(texture);
}

Sprite &Sprite::operator=(Sprite &&spr) {
    if (this != &spr) {
        position = spr.position;
        size = spr.size;
        origin = spr.origin;
        scale = spr.scale;
        rotation = spr.rotation;
        texture = spr.texture;
        cur_animation = spr.cur_animation;
        anim_positions = std::move(spr.anim_positions);
        animations = std::move(spr.animations); 
        
        spr.texture = {};
    }

    return *this;
}

void Sprite::load(const std::string &fname) {
    size_t idx = fname.rfind('.');
    if(idx != std::string::npos) {
        std::string extension = fname.substr(idx + 1);
        if(extension == "json") {
            loadFromJSON(fname);
        }
        else {
            loadFromTexture(fname);
        }
    }
    else {
        err("couldn't find extension while load sprite %s", fname.c_str());
    }
}

void Sprite::load(const std::string &fname, recti rect) {
    loadFromTexture(fname, rect);
}

void Sprite::load(const std::string &fname, i32 rows, i32 columns, f32 frame_duration) {
    loadFromSheet(fname, rows, columns, frame_duration);
}


void Sprite::loadFromTexture(const std::string &fname, recti rect) {
    texture = assets::getTexture(fname);

    if(rect.w && rect.h) {
        animations.init(1);
        anim_positions.init(1);

        animations[0].img_ids.init(1);
        animations[0].duration = INFINITY; // infinite animation
        animations[0].img_ids[0] = 0;
        anim_positions[0] = rect.position();

        size = rect.size();
    }
    else {
        size = {texture.width, texture.height};
    }
}

void Sprite::loadFromSheet(const std::string &fname, i32 rows, i32 columns, f32 frame_duration) {
    texture = assets::getTexture(fname);
    size.x = texture.width / columns;
    size.y = texture.height / rows;

    animations.init(rows);
    anim_positions.init(rows * columns);

    for(i32 r = 0; r < rows; ++r) {
        animations[r].img_ids.init(columns);
        animations[r].duration = frame_duration;
        for(i32 c = 0; c < columns; ++c) {
            u32 index = r * columns + c;
            animations[r].img_ids[c] = index;
            anim_positions[index] = {
                static_cast<f32>(c * size.x),
                static_cast<f32>(r * size.y)
            };
        }
    }
}

void Sprite::loadFromJSON(const std::string &fname) {
    json::Document doc(fname);
    
    texture = assets::getTexture(doc["texture"].getString());
    anim_positions.init(doc["frame_count"].getInteger());
    i64 atlas_width = doc["atlas_width"];
    i64 atlas_height = doc["atlas_height"];

    u32 sprite_width  = texture.width / atlas_width;
    u32 sprite_height = texture.height / atlas_height;

    size.x = sprite_width;
    size.y = sprite_height;

    json::Array json_animations = doc["animations"];

    animations.init(json_animations.size());
    i64 actual_frame_count = 0;
    u32 current_frame = 0;

    for(size_t i = 0; i < json_animations.size(); ++i) {
        json::Object anim  = json_animations[i];
        json::Array frames = anim["frames"];
        f64 time           = anim["time"];
        bool repeat        = anim["repeat"];

        actual_frame_count += frames.size();
        animations[i].img_ids.init(frames.size());
        animations[i].duration = time;
        animations[i].repeat = repeat;
        
        for(size_t j = 0; j < frames.size(); ++j) {
            i64 id = frames[j];
            u32 x = (id % atlas_width) * sprite_width;
            u32 y = (id / atlas_width) * sprite_height;
            anim_positions[current_frame] = {
                static_cast<f32>(x),
                static_cast<f32>(y)
            };
            animations[i].img_ids[j] = current_frame;
            ++current_frame;
        }
    }
    
    if(actual_frame_count != anim_positions.size()) {
        TraceLog(
            LOG_FATAL, 
            "expected number of frames [%d] is different from the actual number of frames [%d]",
            anim_positions.size(),
            actual_frame_count
        );
    }
}

void Sprite::loadNPC(u32 id) {
    texture = assets::getTexture("assets/npcs.png");
    constexpr i32 NPC_WIDTH = 16;
    constexpr i32 NPC_HEIGHT = 24;
    constexpr f32 FRAME_TIME = 0.15f;

    size.x = NPC_WIDTH;
    size.y = NPC_HEIGHT;

    animations.init(8);
    animations[0].img_ids.init(1); animations[0].duration = FRAME_TIME;
    animations[1].img_ids.init(1); animations[1].duration = FRAME_TIME;
    animations[2].img_ids.init(1); animations[2].duration = FRAME_TIME;
    animations[3].img_ids.init(1); animations[3].duration = FRAME_TIME;
    animations[4].img_ids.init(4); animations[4].duration = FRAME_TIME;
    animations[5].img_ids.init(4); animations[5].duration = FRAME_TIME;
    animations[6].img_ids.init(4); animations[6].duration = FRAME_TIME;
    animations[7].img_ids.init(4); animations[7].duration = FRAME_TIME;

    u32 count = 0;
    for(auto &anim: animations) {
        for(auto &id: anim.img_ids) {
            id = count++;
        }
    }

    int animation[] = {
        0, 1, 2, 3,   // standing d-u-l-r
        4, 0, 5, 0,   // walk down
        6, 1, 7, 1,   // walk up
        8, 2, 9, 2,   // walk left
        10, 3, 11, 3, // walk right
    };
    constexpr int len = sizeof(animation)/sizeof(int);
    anim_positions.init(len);

    for(int i = 0; i < len; ++i) {
        printf("i: %d\n", i);
        anim_positions[i] = { (f32)animation[i] * NPC_WIDTH, (f32)id * NPC_HEIGHT };
    }
}

void Sprite::play(u32 animation_id) {
    u32 old_id = cur_animation;
    cur_animation = animation_id;
    Animation &anim = animations[cur_animation];

    if (animation_id != old_id) {
        anim.timer = 0.f;
    }
}

void Sprite::update() {
    if (animations.data() && anim_positions.data()) {
        Animation &anim = animations[cur_animation];
        anim.timer += GetFrameTime();
        while(anim.timer >= anim.duration) {
            anim.timer -= anim.duration;
            anim.cur_image++;
            // wrap animation
            if(anim.finished()) {
                anim.cur_image = anim.repeat ? 0 : (anim.img_ids.size() - 1);
            }
        }
    }
}

void Sprite::updateCallback(AnimFinishedCB callback, void *udata) {
    if (animations.data() && anim_positions.data()) {
        Animation &anim = animations[cur_animation];
        anim.timer += GetFrameTime();
        if (anim.timer >= anim.duration) {
            anim.timer -= anim.duration;
            anim.cur_image++;
            // wrap animation
            if(anim.finished()) {
                callback(*this, udata);
                anim.cur_image = anim.repeat ? 0 : anim.img_ids.size() - 1;
            }
        }
    }
}

void drawSprite(const Sprite &spr) {
    if (spr.texture.id == 0) return;
    vec2f source_pos;
    // if any both the arrays are NOT nullptr, then there is an animation
    if (spr.animations.data() && spr.anim_positions.data()) {
        const Animation &anim = spr.animations[spr.cur_animation];
        u32 id = anim.img_ids[anim.cur_image];
        source_pos = spr.anim_positions[id];
    }

    // convert sprite's frame from ints to floats
    vec2f position = spr.position;
    vec2f size = spr.size;

    Vector2 rl_origin {
        spr.origin.x * spr.scale.x,
        spr.origin.y * spr.scale.y
    };

    Rectangle source {
        source_pos.x, source_pos.y,
        size.x, size.y
    };

    Rectangle dest {
        position.x, position.y,
        size.x * fabsf(spr.scale.x),
        size.y * fabsf(spr.scale.y)
    };

    if(spr.scale.x < 0.f) {
        source.width = -size.x;
        dest.x -= dest.width;
    }
    if(spr.scale.y < 0.f) {
        source.height = -size.y;
        dest.y -= dest.height;
    }

    DrawTexturePro(spr.texture, source, dest, rl_origin, spr.rotation, WHITE);
}