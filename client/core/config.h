#pragma once

#include <core/json.h>
#include <utils/vec.h>

struct Config {
    int tile_size;
    vec2i resolution;

    static void load() {
        Config &conf = Config::get();
        json::Document doc("config.json");

        conf.tile_size = doc["tile_size"].getInteger();    
        conf.resolution.x = doc["resolution"][0].getInteger();
        conf.resolution.y = doc["resolution"][1].getInteger();
    }

    static Config &get() {
        static Config global_conf;
        return global_conf;
    }
};