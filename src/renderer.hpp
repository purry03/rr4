#pragma once
#include "math.hpp"
#include <atomic>
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include <cmath>

namespace rr4 {

inline uint32_t packRGBA(int r, int g, int b, int a = 255) {
    return uint32_t(clampf(float(r), 0, 255)) | (uint32_t(clampf(float(g), 0, 255)) << 8) |
           (uint32_t(clampf(float(b), 0, 255)) << 16) | (uint32_t(clampf(float(a), 0, 255)) << 24);
}
inline void unpackRGBA(uint32_t c, int &r, int &g, int &b, int &a) {
    r = c & 255; g = (c >> 8) & 255; b = (c >> 16) & 255; a = (c >> 24) & 255;
}

inline bool gSmoothTextures = true;

inline uint32_t nextTextureVersion() {
    static std::atomic<uint32_t> counter{0};
    return counter.fetch_add(1, std::memory_order_relaxed) + 1;
}

struct Texture {
    int w = 0, h = 0;
    // Unique per content instance; reassigned whenever px or mips change. The
    // GL renderer caches uploads per pixel-buffer address, and a reloaded
    // buffer can reuse the same address (the lazy InfoPanels cache does), so
    // the version decides when to re-upload.
    uint32_t version = nextTextureVersion();
    std::vector<uint32_t> px;
    // Downsampled copies (half size each), mips[0] is w/2 x h/2. Empty until
    // buildMips(); sampling then always uses the base level.
    std::vector<Texture> mips;

    void buildMips();
    int levels() const { return 1 + int(mips.size()); }
    const Texture &level(int l) const { return l <= 0 ? *this : mips[size_t(std::min(l, int(mips.size())) - 1)]; }

    uint32_t sample(float u, float v, bool smooth = true) const {
        if (px.empty()) return packRGBA(255, 0, 255);
        if (!smooth) {
            int x = wrap(ifloor(u * float(w)), w);
            int y = wrap(ifloor(v * float(h)), h);
            return px[size_t(y) * w + x];
        }
        // Bilinear filtering on texel centres.
        float fx = u * float(w) - 0.5f;
        float fy = v * float(h) - 0.5f;
        int xi = ifloor(fx), yi = ifloor(fy);
        int fx8 = int((fx - float(xi)) * 256.0f);
        int fy8 = int((fy - float(yi)) * 256.0f);
        int x0 = wrap(xi, w), y0 = wrap(yi, h);
        int x1 = x0 + 1; if (x1 >= w) x1 = 0;
        int y1 = y0 + 1; if (y1 >= h) y1 = 0;
        return bilerp(x0, y0, x1, y1, fx8, fy8);
    }

    // Clamped bilinear sample for screen sprites (no wrap-around bleeding).
    uint32_t sampleClamped(float u, float v) const {
        float fx = std::clamp(u * float(w) - 0.5f, 0.0f, float(w - 1));
        float fy = std::clamp(v * float(h) - 0.5f, 0.0f, float(h - 1));
        int x0 = int(fx), y0 = int(fy);
        int fx8 = int((fx - float(x0)) * 256.0f);
        int fy8 = int((fy - float(y0)) * 256.0f);
        return bilerp(x0, y0, std::min(x0 + 1, w - 1), std::min(y0 + 1, h - 1), fx8, fy8);
    }

private:
    // Hot-path helpers: std::floor and % are library calls / divisions here.
    static int ifloor(float f) { int i = int(f); return i - (float(i) > f); }
    static int wrap(int i, int n) {
        if ((n & (n - 1)) == 0) return i & (n - 1);
        i %= n; return i < 0 ? i + n : i;
    }
    uint32_t bilerp(int x0, int y0, int x1, int y1, int fx8, int fy8) const {
        const uint32_t *row0 = px.data() + size_t(y0) * w;
        const uint32_t *row1 = px.data() + size_t(y1) * w;
        uint32_t c00 = row0[x0], c10 = row0[x1], c01 = row1[x0], c11 = row1[x1];
        // Weights sum to 256; red/blue and green/alpha are filtered as two
        // packed 16-bit lanes each, so the four channels need 8 multiplies.
        uint32_t w11 = uint32_t(fx8 * fy8) >> 8;
        uint32_t w10 = uint32_t(fx8) - w11;
        uint32_t w01 = uint32_t(fy8) - w11;
        uint32_t w00 = 256u - w10 - w01 - w11;
        constexpr uint32_t m = 0x00ff00ffu;
        uint32_t rb = ((c00 & m) * w00 + (c10 & m) * w10 + (c01 & m) * w01 + (c11 & m) * w11) >> 8;
        uint32_t ga = ((c00 >> 8) & m) * w00 + ((c10 >> 8) & m) * w10 +
                      ((c01 >> 8) & m) * w01 + ((c11 >> 8) & m) * w11;
        return (rb & m) | (ga & ~m);
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

// One light of the original scene: kind 0 ambient, 1 directional (toward is
// the unit vector pointing at the light).
struct SceneLight {
    int kind;
    Vec3 color;
    Vec3 toward;
};

// Director #standard shader lighting for one draw (per-vertex, Gouraud).
// The lit colour is emissive + ambientLight * ambient + sum(light * diffuse *
// N.L); a textured shader ignores its diffuse colour (useDiffuseWithTexture
// is FALSE), so diffuse is white then. Specular is added after texturing.
// `reflect` > 0 blends a sphere-mapped reflection texture over the result.
struct SurfaceLighting {
    Vec3 ambient{0.25f, 0.25f, 0.25f};
    Vec3 diffuse{1.0f, 1.0f, 1.0f};
    Vec3 specular{0.0f, 0.0f, 0.0f};
    Vec3 emissive{0.0f, 0.0f, 0.0f};
    float shininess = 0.3f;          // 0..1, OpenGL exponent = shininess * 128
    float reflect = 0.0f;            // texture layer #blend constant
    const Texture *reflectTex = nullptr;
};

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

// Texture sub-rectangle and colour for a screen sprite (font atlas glyphs,
// tinted text). The defaults draw the whole texture untinted.
struct SpriteStyle {
    float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;
    uint32_t tint = 0xffffffffu;   // multiplies the texture colour
};

// Rendering interface shared by the software rasterizer and the OpenGL
// backend. Draw calls are queued in submission order and resolved by
// flush(); per-pixel draw order follows submission order in both backends.
class Renderer {
public:
    int width, height;

    Renderer(int w, int h) : width(w), height(h) {}
    virtual ~Renderer() = default;

    // Start a frame: clear to the sky gradient.
    virtual void clearSky(const Vec3 &camPos, float pitch = 0.0f) = 0;
    virtual void drawMesh(const Camera &cam, const Mat4 &model, const Mesh &mesh,
                          const Texture *tex, uint32_t tint = 0xffffffff,
                          bool unlit = false, int cull = 1, float alpha = 1.0f,
                          bool depthWrite = true, size_t idxBegin = 0,
                          size_t idxCount = size_t(-1)) = 0;
    // Soft blob under a car; backends with real shadows may skip it.
    virtual void drawShadow(const Camera &cam, const Vec3 &pos, float radius) = 0;
    // Screen-space sprite for the original Director camera overlays (HUD).
    // x/y/w/h are in framebuffer pixels; alpha multiplies the texture alpha.
    virtual void drawSprite(const Texture &tex, float x, float y, float w, float h,
                            float alpha = 1.0f, const SpriteStyle &style = {}) = 0;
    // Solid screen rectangle (debug text), alpha-blended.
    virtual void drawRect(int x, int y, int w, int h, uint32_t color, float alpha = 1.0f) = 0;
    // Resolve everything queued so far.
    virtual void flush() = 0;
    // The finished frame as RGBA (packRGBA layout), top row first.
    virtual void readPixels(std::vector<uint32_t> &out) = 0;

    // World fog. World::draw sets the range from its draw distance so the fog
    // is solid before the edge of the drawn tiles; setWorldFog() applies it.
    static constexpr uint32_t kWorldFogColor = 0xffe2cbb0u;   // packRGBA(176, 203, 226)
    void setWorldFogRange(float start, float end) { worldFogStart_ = start; worldFogEnd_ = end; }
    void setWorldFog() { setFog(kWorldFogColor, worldFogStart_, worldFogEnd_); }
    void setFog(uint32_t color, float start, float end) { fogColor_ = color; fogStart_ = start; fogEnd_ = end; }
    // Scene lights used by every lit draw.
    void setLights(const SceneLight *lights, int count) { lights_.assign(lights, lights + count); }
    // Lighting for the next drawMesh calls; resetSurface() restores the
    // default material (white diffuse, ambient 0.25, no specular).
    void setSurface(const SurfaceLighting &s) { surface_ = s; }
    void resetSurface() { surface_ = SurfaceLighting{}; }
    // Shader textureTransform translation added to every UV of the next
    // drawMesh calls (Director texture scrolling). Reset to 0 after use.
    void setUVOffset(float du, float dv) { uOffset_ = du; vOffset_ = dv; }

    // Sky gradient (clearSky): top colour to horizon colour, t^0.75.
    static constexpr uint32_t kSkyTop = 0xffbe7a40u;      // packRGBA(64, 122, 190)
    static constexpr uint32_t kSkyHorizon = 0xffeedac4u;  // packRGBA(196, 218, 238)
    static constexpr float kSkyCurve = 0.75f;

protected:
    float worldFogStart_ = 2500.0f, worldFogEnd_ = 9500.0f;
    uint32_t fogColor_ = packRGBA(158, 196, 226);
    float fogStart_ = 4000.0f, fogEnd_ = 16000.0f;
    float uOffset_ = 0.0f, vOffset_ = 0.0f;
    std::vector<SceneLight> lights_;
    SurfaceLighting surface_;
};

// Software renderer. drawMesh() transforms, clips and sets up triangles and
// queues them; flush() rasterizes the queue on all cores, split into
// horizontal strips so every pixel still sees its triangles in submission
// order. Anything that reads or writes color/depth directly must flush first
// (drawSprite and drawRect do).
class SoftRenderer final : public Renderer {
public:
    std::vector<uint32_t> color;
    std::vector<float> depth;

    SoftRenderer(int w, int h);

    void clearSky(const Vec3 &camPos, float pitch = 0.0f) override;
    void drawMesh(const Camera &cam, const Mat4 &model, const Mesh &mesh,
                  const Texture *tex, uint32_t tint = 0xffffffff,
                  bool unlit = false, int cull = 1, float alpha = 1.0f,
                  bool depthWrite = true, size_t idxBegin = 0,
                  size_t idxCount = size_t(-1)) override;
    void drawShadow(const Camera &cam, const Vec3 &pos, float radius) override;
    void drawSprite(const Texture &tex, float x, float y, float w, float h,
                    float alpha = 1.0f, const SpriteStyle &style = {}) override;
    void drawRect(int x, int y, int w, int h, uint32_t color, float alpha = 1.0f) override;
    void flush() override;
    void readPixels(std::vector<uint32_t> &out) override;

private:
    struct ClipVert {
        Vec3 view;   // camera space
        float u, v;
        float fog;
        Vec3 lit;    // lit colour (texture multiplier), 0..1
        Vec3 spec;   // specular, added after texturing
        float ru, rv; // reflection map coordinates
    };
    // Per drawMesh call state shared by its triangles.
    struct DrawState {
        const Texture *tex;
        int tr, tg, tb;     // tint, 0..255
        int alpha256;       // material alpha * tint alpha, 0..256
        uint32_t fogColor;
        bool unlit, depthWrite;
        bool hasSpec;
        int reflect256;              // reflection blend, 0..256
        const Texture *reflectTex;
    };
    // A triangle after setup: each attribute is a screen-space plane
    // a(x, y) = c + dx * x + dy * y, pre-divided by view depth.
    struct Plane { float c, dx, dy; };
    struct RasterTri {
        float ex[3], ey[3], ec[3];   // edge functions, normalised barycentrics
        Plane iz, uz, vz, fz;
        Plane lr{}, lg{}, lb{};      // lit colour
        Plane sr{}, sg{}, sb{};      // specular (when hasSpec)
        Plane ru{}, rv{};            // reflection uv (when reflect256 > 0)
        int minX, maxX, minY, maxY;
        uint32_t state;
    };

    void setupTri(const ClipVert &a, const ClipVert &b, const ClipVert &c, int cull);
    void rasterStrip(const RasterTri &t, int y0, int y1);

    std::vector<DrawState> states_;
    std::vector<RasterTri> tris_;
    std::vector<std::vector<uint32_t>> bins_;
    std::vector<uint32_t> skyRow_;
    bool clearPending_ = false;

    std::vector<ClipVert> vcache_;
    std::vector<uint32_t> vstamp_;
    uint32_t stamp_ = 0;

    float projX_ = 1.0f, projY_ = 1.0f;   // focal length / aspect, focal length
};

} // namespace rr4
