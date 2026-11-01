#pragma once
#include "renderer.hpp"
#include <string>

namespace rr4 {

// 5 x 7 bitmap font, drawn straight into the software framebuffer.
void drawText(Renderer &r, int x, int y, const std::string &text,
              uint32_t color, int scale = 2);
int textWidth(const std::string &text, int scale = 2);
void fillRect(Renderer &r, int x, int y, int w, int h, uint32_t color);
void blendRect(Renderer &r, int x, int y, int w, int h, uint32_t color, float alpha);

} // namespace rr4
