#pragma once

#include <string>
#include <raylib.h>

#include "ref_map.h"

struct TextureLoader {
    using ValueType = Texture2D;
    
    static Texture2D load(const std::string &key) {
        return LoadTexture(key.c_str());
    }

    static void unload(const Texture2D &texture) {
        UnloadTexture(texture);
    }

    static bool isValid(const Texture2D &texture) {
        return texture.id != 0;
    }

    static bool areSame(const Texture2D &a, const Texture2D &b) {
        return a.id == b.id;
    }
    
    static void errorNotFound(const Texture2D &texture) {
        TraceLog(LOG_ERROR, "texture id [%u] wasn't loaded", texture.id);
    }
    
    static void errorForgot(const Texture2D &texture) {
        TraceLog(LOG_ERROR, "forgotten to unload texture id [%u]", texture.id);
    }
};

using TextureMap = RefMap<TextureLoader>;
