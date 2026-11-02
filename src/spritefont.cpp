#include "spritefont.hpp"
#include "assets.hpp"
#include "hud.hpp"

#include <cctype>
#include <fstream>
#include <string>

namespace rr4 {

bool SpriteFont::load(const std::string &dir) {
    if (!loadTexture(dir + "/Font.png", atlas_)) return false;
    atlas_.buildMips();
    std::ifstream in(dir + "/Font.txt");
    std::string key;
    if (!(in >> key >> height_) || key != "height") return false;
    int code, x, w;
    while (in >> code >> x >> w)
        if (code >= 0 && code < 128) glyphs_[code] = {x, w};
    return true;
}

float SpriteFont::width(const std::string &text, float size) const {
    const float scale = size / float(height_);
    float w = 0.0f;
    for (char ch : text) {
        const int c = std::toupper(static_cast<unsigned char>(ch));
        if (c >= 0 && c < 128) w += float(glyphs_[c].w) * scale;
    }
    return w;
}

void SpriteFont::draw(Renderer &r, const std::string &text, float x, float y, float size,
                      uint32_t color, float alpha, Align align) const {
    if (atlas_.px.empty() || alpha <= 0.0f) return;
    if (align == Align::Centre) x -= width(text, size) * 0.5f;
    else if (align == Align::Right) x -= width(text, size);
    const float scale = size / float(height_);
    for (char ch : text) {
        const int c = std::toupper(static_cast<unsigned char>(ch));
        if (c < 0 || c >= 128 || glyphs_[c].w == 0) continue;
        const Glyph &g = glyphs_[c];
        if (c != ' ') {
            SpriteStyle st;
            st.u0 = float(g.x) / float(atlas_.w);
            st.u1 = float(g.x + g.w) / float(atlas_.w);
            st.tint = color;
            drawStageSpriteRect(r, atlas_, x, y, float(g.w) * scale, size, alpha, st);
        }
        x += float(g.w) * scale;
    }
}

} // namespace rr4
