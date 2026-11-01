#pragma once
#include "math.hpp"
#include <vector>
#include <cstdint>
#include <string>

namespace rr4 {

inline uint32_t packRGBA(int r, int g, int b, int a = 255) {
    return uint32_t(clampf(float(r), 0, 255)) | (uint32_t(clampf(float(g), 0, 255)) << 8) |
           (uint32_t(clampf(float(b), 0, 255)) << 16) | (uint32_t(clampf(float(a), 0, 255)) << 24);
}
inline void unpackRGBA(uint32_t c, int &r, int &g, int &b, int &a) {
    r = c & 255; g = (c >> 8) & 255; b = (c >> 16) & 255; a = (c >> 24) & 255;
}

inline bool gSmoothTextures = true;

struct Texture {
    int w = 0, h = 0;
    std::vector<uint32_t> px;
    uint32_t sample(float u, float v, bool smooth = true) const {
        if (px.empty()) return packRGBA(255, 0, 255);
        if (!smooth) {
            int x = int(u * float(w));
            int y = int(v * float(h));
            if ((w & (w - 1)) == 0) x &= (w - 1); else { x %= w; if (x < 0) x += w; }
            if ((h & (h - 1)) == 0) y &= (h - 1); else { y %= h; if (y < 0) y += h; }
            return px[size_t(y) * w + x];
        }
        // Bilinear filtering: the original 3DOM textures are tiny (often
        // 32x16), so nearest sampling would show giant texel blocks.
        float fx = u * float(w);
        float fy = v * float(h);
        int xi = int(fx), yi = int(fy);
        int fx8 = int((fx - float(xi)) * 256.0f);
        int fy8 = int((fy - float(yi)) * 256.0f);
        int x0 = xi % w; if (x0 < 0) x0 += w;
        int y0 = yi % h; if (y0 < 0) y0 += h;
        int x1 = x0 + 1; if (x1 >= w) x1 = 0;
        int y1 = y0 + 1; if (y1 >= h) y1 = 0;

        const uint32_t *row0 = px.data() + size_t(y0) * w;
        const uint32_t *row1 = px.data() + size_t(y1) * w;
        uint32_t c00 = row0[x0], c10 = row0[x1], c01 = row1[x0], c11 = row1[x1];
        int wx0 = 256 - fx8, wx1 = fx8;
        int wy0 = 256 - fy8, wy1 = fy8;
        int w00 = wx0 * wy0, w10 = wx1 * wy0, w01 = wx0 * wy1, w11 = wx1 * wy1;
        int r = (int(c00 & 255) * w00 + int(c10 & 255) * w10 +
                 int(c01 & 255) * w01 + int(c11 & 255) * w11) >> 16;
        int g = (int((c00 >> 8) & 255) * w00 + int((c10 >> 8) & 255) * w10 +
                 int((c01 >> 8) & 255) * w01 + int((c11 >> 8) & 255) * w11) >> 16;
        int b = (int((c00 >> 16) & 255) * w00 + int((c10 >> 16) & 255) * w10 +
                 int((c01 >> 16) & 255) * w01 + int((c11 >> 16) & 255) * w11) >> 16;
        int a = (int(c00 >> 24) * w00 + int(c10 >> 24) * w10 +
                 int(c01 >> 24) * w01 + int(c11 >> 24) * w11) >> 16;
        return uint32_t(r) | (uint32_t(g) << 8) | (uint32_t(b) << 16) | (uint32_t(a) << 24);
    }
};

Texture makeAsphaltTexture(int size = 128);
Texture makeGrassTexture(int size = 128);
Texture makeConcreteTexture(int size = 128);
Texture makeWaterTexture(int size = 128);
Texture makeCarTexture(int size = 128);
Texture makeFacadeTexture(int size = 128);
Texture makeShadowTexture(int size = 64);
Texture makeSkyGradient(int w, int h);

struct Vertex {
    Vec3 p;      // local position
    Vec3 n;      // local normal
    float u = 0, v = 0;
};

struct Mesh {
    std::vector<Vertex> verts;
    std::vector<uint32_t> idx;

    void addQuad(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d,
                 const Vec3 &n, float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 1);
    void addBox(const Vec3 &center, const Vec3 &size, const Vec3 &uvScale = {1, 1, 1});
    void addCylinderX(const Vec3 &center, float radius, float width, int sides, const Vec3 &uvScale = {1, 1, 1});
    void addTri(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &n);
};

struct Camera {
    Vec3 pos{0, 200, -320};
    Vec3 target{0, 90, 0};
    Vec3 up{0, 1, 0};
    float fovDeg = 55.0f;
    float nearZ = 2.0f;
    float farZ = 60000.0f;

    Mat4 view() const { return Mat4::lookAt(pos, target, up); }
    Mat4 projection(float aspect) const;
};

class Renderer {
public:
    int width, height;
    std::vector<uint32_t> color;
    std::vector<float> depth;

    Renderer(int w, int h) : width(w), height(h), color(size_t(w) * h), depth(size_t(w) * h) {}

    void clearSky(const Vec3 &camPos, float pitch = 0.0f);
    void drawMesh(const Camera &cam, const Mat4 &model, const Mesh &mesh,
                  const Texture *tex, uint32_t tint = 0xffffffff,
                  bool unlit = false, int cull = 1, float alpha = 1.0f,
                  bool depthWrite = true, size_t idxBegin = 0, size_t idxCount = size_t(-1));
    void drawShadow(const Camera &cam, const Vec3 &pos, float radius);

    // Screen-space sprite for the original Director camera overlays (HUD).
    // x/y/w/h are in framebuffer pixels; alpha multiplies the texture alpha.
    void drawSprite(const Texture &tex, float x, float y, float w, float h,
                    float alpha = 1.0f);

    void setFog(uint32_t color, float start, float end) { fogColor_ = color; fogStart_ = start; fogEnd_ = end; }

private:
    Mat4 view_{};
    Mat4 proj_{};
    Vec3 camPos_{};
    float aspect_ = 1.0f;
    uint32_t fogColor_ = packRGBA(158, 196, 226);
    float fogStart_ = 4000.0f, fogEnd_ = 16000.0f;

    struct ClipVert {
        Vec3 view;   // camera space
        Vec3 normal; // world space
        float u, v;
        float fog;
        float shade;
    };
    void rasterize(ClipVert a, ClipVert b, ClipVert c,
                   const Texture *tex, uint32_t tint, bool unlit, float alpha, bool depthWrite,
                   int cull);
    void project(const ClipVert &v, float &sx, float &sy, float &sz) const;
    float fov_ = 55.0f;
};

} // namespace rr4
