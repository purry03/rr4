#pragma once
// Runtime text from the Speedway BV font atlas made by tools/render_menu.py
// (assets/hud/menu/Font.png + Font.txt). Text is drawn in 800 x 480 stage
// coordinates; `size` is the line height in stage pixels.

#include "renderer.hpp"

#include <string>

namespace rr4 {

class SpriteFont {
public:
    enum class Align { Left, Centre, Right };

    bool load(const std::string &dir = "assets/hud/menu");
    float width(const std::string &text, float size) const;
    void draw(Renderer &r, const std::string &text, float x, float y, float size,
              uint32_t color = 0xffffffffu, float alpha = 1.0f, Align align = Align::Left) const;

private:
    struct Glyph { int x = 0, w = 0; };
    Texture atlas_;
    int height_ = 1;
    Glyph glyphs_[128];
};

} // namespace rr4
