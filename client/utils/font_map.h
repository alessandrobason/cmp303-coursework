#pragma once

#include <string>
#include <raylib.h>

#include "ref_map.h"

struct FontLoader {
    using ValueType = Font;
    
    static Font load(const std::string &key) {
        TraceLog(LOG_INFO, "FONTMAP: loading from only string");
        return LoadFont(key.c_str());
    }
    
    static Font load(const std::string &key, int font_size, int *font_chars = NULL, int glyph_count = 96) {
        TraceLog(LOG_INFO, "FONTMAP: loading from full data");
        return LoadFontEx(key.c_str(), font_size, font_chars, glyph_count);
    }

    static void unload(const Font &font) {
        UnloadFont(font);
    }

    static bool isValid(const Font &font) {
        return font.texture.id != 0;
    }

    static bool areSame(const Font &a, const Font &b) {
        return a.texture.id == b.texture.id;
    }
    
    static void errorNotFound(const Font &font) {
        TraceLog(LOG_FATAL, "font id [%u] wasn't loaded", font.texture.id);
    }
    
    static void errorForgot(const Font &font) {
        TraceLog(LOG_ERROR, "forgotten to unload font id [%u]", font.texture.id);
    }
};

using FontMap = RefMap<FontLoader>;
