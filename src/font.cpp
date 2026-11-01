#include "font.hpp"
#include <array>
#include <cctype>
#include <cmath>

namespace rr4 {

namespace {

struct Glyph {
    char c;
    std::array<uint8_t, 7> rows; // 5 bits, bit 4 = leftmost pixel
};

const Glyph kGlyphs[] = {
    {'A', {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
    {'B', {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110}},
    {'C', {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110}},
    {'D', {0b11100, 0b10010, 0b10001, 0b10001, 0b10001, 0b10010, 0b11100}},
    {'E', {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111}},
    {'F', {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000}},
    {'G', {0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111}},
    {'H', {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
    {'I', {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111}},
    {'J', {0b00111, 0b00010, 0b00010, 0b00010, 0b00010, 0b10010, 0b01100}},
    {'K', {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001}},
    {'L', {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111}},
    {'M', {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001}},
    {'N', {0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001}},
    {'O', {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}},
    {'P', {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000}},
    {'Q', {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101}},
    {'R', {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001}},
    {'S', {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110}},
    {'T', {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100}},
    {'U', {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}},
    {'V', {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100}},
    {'W', {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b11011, 0b10001}},
    {'X', {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001}},
    {'Y', {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100}},
    {'Z', {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111}},
    {'0', {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110}},
    {'1', {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110}},
    {'2', {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111}},
    {'3', {0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110}},
    {'4', {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010}},
    {'5', {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110}},
    {'6', {0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110}},
    {'7', {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000}},
    {'8', {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110}},
    {'9', {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100}},
    {' ', {0, 0, 0, 0, 0, 0, 0}},
    {'.', {0, 0, 0, 0, 0, 0b01100, 0b01100}},
    {',', {0, 0, 0, 0, 0b01100, 0b00100, 0b01000}},
    {':', {0, 0b00110, 0b00110, 0, 0b00110, 0b00110, 0}},
    {'-', {0, 0, 0, 0b11111, 0, 0, 0}},
    {'/', {0b00001, 0b00010, 0b00010, 0b00100, 0b01000, 0b01000, 0b10000}},
    {'%', {0b11001, 0b11010, 0b00010, 0b00100, 0b01000, 0b01011, 0b10011}},
    {'+', {0, 0b00100, 0b00100, 0b11111, 0b00100, 0b00100, 0}},
    {'\'', {0b00100, 0b00100, 0b01000, 0, 0, 0, 0}},
    {'!', {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0, 0b00100}},
    {'?', {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0, 0b00100}},
    {'(', {0b00010, 0b00100, 0b01000, 0b01000, 0b01000, 0b00100, 0b00010}},
    {')', {0b01000, 0b00100, 0b00010, 0b00010, 0b00010, 0b00100, 0b01000}},
    {'=', {0, 0, 0b11111, 0, 0b11111, 0, 0}},
    {'>', {0b01000, 0b00100, 0b00010, 0b00001, 0b00010, 0b00100, 0b01000}},
    {'<', {0b00010, 0b00100, 0b01000, 0b10000, 0b01000, 0b00100, 0b00010}},
    {'*', {0, 0b10101, 0b01110, 0b11111, 0b01110, 0b10101, 0}},
};

const Glyph *findGlyph(char c) {
    c = char(std::toupper(static_cast<unsigned char>(c)));
    for (const Glyph &g : kGlyphs)
        if (g.c == c) return &g;
    return nullptr;
}

int textAdvance(int scale) { return 6 * scale; }

} // namespace

int textWidth(const std::string &text, int scale) {
    return int(text.size()) * textAdvance(scale);
}

void drawText(Renderer &r, int x, int y, const std::string &text,
              uint32_t color, int scale) {
    int cx = x;
    for (char ch : text) {
        if (ch == '\n') { y += 8 * scale; cx = x; continue; }
        const Glyph *g = findGlyph(ch);
        if (g) {
            for (int row = 0; row < 7; ++row) {
                uint8_t bits = g->rows[row];
                for (int col = 0; col < 5; ++col) {
                    if (!(bits & (1u << (4 - col)))) continue;
                    fillRect(r, cx + col * scale, y + row * scale, scale, scale, color);
                }
            }
        }
        cx += textAdvance(scale);
    }
}

void fillRect(Renderer &r, int x, int y, int w, int h, uint32_t color) {
    r.flush();
    int x0 = std::max(0, x), y0 = std::max(0, y);
    int x1 = std::min(r.width, x + w), y1 = std::min(r.height, y + h);
    if (x1 <= x0 || y1 <= y0) return;
    for (int yy = y0; yy < y1; ++yy)
        std::fill(r.color.begin() + size_t(yy) * r.width + x0,
                  r.color.begin() + size_t(yy) * r.width + x1, color);
}

void blendRect(Renderer &r, int x, int y, int w, int h, uint32_t color, float alpha) {
    r.flush();
    int x0 = std::max(0, x), y0 = std::max(0, y);
    int x1 = std::min(r.width, x + w), y1 = std::min(r.height, y + h);
    if (x1 <= x0 || y1 <= y0) return;
    int sr = color & 255, sg = (color >> 8) & 255, sb = (color >> 16) & 255;
    for (int yy = y0; yy < y1; ++yy) {
        uint32_t *row = r.color.data() + size_t(yy) * r.width;
        for (int xx = x0; xx < x1; ++xx) {
            uint32_t d = row[xx];
            int dr = d & 255, dg = (d >> 8) & 255, db = (d >> 16) & 255;
            row[xx] = packRGBA(int(sr * alpha + dr * (1 - alpha)),
                               int(sg * alpha + dg * (1 - alpha)),
                               int(sb * alpha + db * (1 - alpha)));
        }
    }
}

} // namespace rr4
