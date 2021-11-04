#include "assets.h"

#include <utils/texture_map.h>
#include <utils/font_map.h>

static TextureMap texture_cache;
static FontMap font_cache;

namespace assets {
    Texture2D getTexture(const std::string &fname) {
        return texture_cache.load(fname);
    }

    bool destroyTexture(Texture2D &texture) {
        bool result = texture_cache.tryUnload(texture);
        memset(&texture, 0, sizeof(texture));
        return result;
    }

    Font getFont(const std::string &fname) {
        return font_cache.load(fname);
    }

    Font getFont(const std::string &fname, int font_size, int *font_chars, int glyph_count) {
        return font_cache.load(fname, font_size, font_chars, glyph_count);
    }

    bool destroyFont(Font &font) {
        bool result = font_cache.tryUnload(font);
        memset(&font, 0, sizeof(font));
        return result;
    }
} // namespace assets
