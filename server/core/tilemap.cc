#include "tilemap.h"

#include <stdio.h>
#include <assert.h>

#include <core/assets.h>
#include <core/config.h>
#include <core/json.h>
#include <tracelog.h>

enum {
    COLLISION_RECT, COLLISION_CIRCLE
};

Tilemap::~Tilemap() {
    assets::destroyTexture(tileset.texture);
}

void Tilemap::onExit() {
    assets::destroyTexture(tileset.texture);
}

void Tilemap::loadFromJSON(const std::string &fname) {
    info("Loading map from json: %s", fname.c_str());
    
    // already loaded
    if(tileset.texture.id != 0) return;

    json::Document doc(fname);

    json::Object json_tileset = doc["tileset"];
    json::Object tilemap = doc["tilemap"];

    // loading tileset
    {
        tileset.texture = assets::getTexture(json_tileset["texture"].getString());
        tileset.size = {
            tileset.texture.width / Config::get().tile_size,
            tileset.texture.height / Config::get().tile_size,
        };
    }

    // loading tilemap
    {
        size = {
            (int)tilemap["width"].getInteger(),
            (int)tilemap["height"].getInteger(),
        };
        layer_count = tilemap["layer_count"].getInteger();
        layers_under = tilemap["layers_under"].getInteger();

        if(layers_under > layer_count) {
            fatal("layers_under is higher than layer_count");
        }

        // loading tiles
        {
            json::Array json_tiles = tilemap["tiles"];
            u32 expected_count = size.x * size.y * layer_count;

            if(expected_count != json_tiles.size()) {
                fatal(
                    "number of tiles [%d] is different then the expected size [%d]",
                    json_tiles.size(), expected_count
                );
            }

            tiles.init(json_tiles.size());

            for(int i = 0; i < json_tiles.size(); ++i) {
                tiles[i] = json_tiles[i].getInteger();
            }
        }

        // loading collisions
        {
            json::Array collisions = tilemap["collisions"];
            colliders.init();

            for(size_t i = 0; i < collisions.size(); ++i) {
                auto &coll = collisions[i];

                vec2i pos {
                    (i32)coll["x"].getInteger(),
                    (i32)coll["y"].getInteger(),
                };

                if(coll["type"].getInteger() == COLLISION_RECT) {
                    vec2i size {
                        (i32)coll["w"].getInteger(),
                        (i32)coll["h"].getInteger(),
                    };
                    colliders.setFixtureAsBox(size, pos + size / 2);
                }
                else {
                    colliders.setFixtureAsCircle(
                        pos, 
                        (i32)coll["radius"].getInteger()
                    );
                }
            }
        }

        // loading animations
        {
            json::Array json_animations = tilemap["animations"];
            animations.init(json_animations.size());

            for(int i = 0; i < json_animations.size(); ++i) {
                json::Object json_anim = json_animations[i];
                f64 json_duration = json_anim["duration"];
                json::Array json_animation = json_anim["animation"];
                json::Array json_indices   = json_anim["indices"];

                TileAnimation &anim = animations[i];
                anim.duration = json_duration;
                anim.animation.init(json_animation.size());
                anim.tiles.init(json_indices.size());

                // load animation
                {
                    for(size_t j = 0; j < json_animation.size(); ++j) {
                        anim.animation[j] = json_animation[j].getInteger();
                    }
                }

                // load indices
                {
                    for(size_t j = 0; j < json_indices.size(); ++j) {
                        anim.tiles[j] = json_indices[j].getInteger();
                    }
                }
            }
        }
    }
}

vec2i Tilemap::mapSizePixels() const { 
    return size * Config::get().tile_size; 
}

recti Tilemap::getBounds() const {
    return {
        position,
        (size * Config::get().tile_size) - 1
    };
}

i32 Tilemap::positionToIndex(const vec2i &pos) const {
    if (!getBounds().contains(pos)) {
        return -1;
    }

    i32 x = (pos.x - position.x) / Config::get().tile_size;
    i32 y = (pos.y - position.y) / Config::get().tile_size;

    return (y * size.x) + x;
}

void Tilemap::update() {
    if(animations.data()) {
        for(auto &anim: animations) {
            anim.timer += GetFrameTime();
            while(anim.timer >= anim.duration) {
                anim.timer -= anim.duration;
                // wrap animation
                anim.cur_frame = (anim.cur_frame + 1) % anim.animation.size();
                i32 new_frame = anim.animation[anim.cur_frame];
                for(u32 tile: anim.tiles) {
                    tiles[tile] = new_frame;
                }
            }
        }
    }
}


Tilemap &Tilemap::operator=(Tilemap &&other) {
    tileset = other.tileset;
    position = other.position;
    size = other.size;
    layer_count = other.layer_count;
    layers_under = other.layers_under;
    tiles = std::move(other.tiles);
    animations = std::move(other.animations);

    // set the id to 0 so the texture doesn't get unloaded
    other.tileset.texture.id = 0;

    return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////

void drawTilemapUnder(Tilemap &tm) {
    for(i32 layer = 0; layer < tm.layers_under; ++layer) {
        drawTilemapLayer(tm, layer);
    }
}

void drawTilemapOver(Tilemap &tm) {
    for(i32 layer = tm.layers_under; layer < tm.layer_count; ++layer) {
        drawTilemapLayer(tm, layer);
    }
}

void drawTilemapLayer(Tilemap &tm, i32 layer) {
    const i32 map_w = tm.size.x; // map width
    const i32 map_h = tm.size.y; // map height
    const i32 ts_w = tm.tileset.size.x; // tileset width
    const f32 ftile_size = static_cast<f32>(Config::get().tile_size);
    const u32 layer_size = map_w * map_h;
    const u32 layer_offset = layer * layer_size;

    for(i32 row = 0; row < map_h; ++row) {
        const u32 row_offset = row * map_w;
        for(i32 col = 0; col < map_w; ++col) {
            u32 index = col + row_offset + layer_offset;
            i32 tile = tm.tiles[index];
            if(tile < 0) continue;
            i32 src_x = tile % ts_w * Config::get().tile_size;
            i32 src_y = tile / ts_w * Config::get().tile_size;
            
            i32 dest_x = tm.position.x + (col * Config::get().tile_size);
            i32 dest_y = tm.position.y + (row * Config::get().tile_size);
            
            DrawTexturePro(
                tm.tileset.texture,
                {
                    static_cast<f32>(src_x),
                    static_cast<f32>(src_y),
                    ftile_size, ftile_size
                },
                {
                    static_cast<f32>(dest_x),
                    static_cast<f32>(dest_y),
                    ftile_size, ftile_size
                },
                {0, 0},
                0.f,
                WHITE
            );
        }
    }
}

void drawTilemap(Tilemap &tm) {
    for(i32 layer = 0; layer < tm.layer_count; ++layer) {
        drawTilemapLayer(tm, layer);
    }
}