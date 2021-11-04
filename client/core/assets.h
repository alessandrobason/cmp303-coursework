#pragma once

#include <string>
#include <raylib.h>

namespace assets {
    Texture2D getTexture(const std::string &fname);
    bool destroyTexture(Texture2D &texture);

    Font getFont(const std::string &fname);
    Font getFont(const std::string &fname, int font_size, int *font_chars = NULL, int glyph_count = 96);
    bool destroyFont(Font &font);
} // namespace assets