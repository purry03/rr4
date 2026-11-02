#pragma once
// OpenGL 2.1 (GLSL 1.20) backend for the Renderer interface.
//
// drawMesh / drawSprite / drawRect only record commands; flush() replays them
// in submission order, so the frame matches the software rasterizer's draw
// order. Lighting is the same per-vertex Director model as SoftRenderer
// (src/renderer.cpp), evaluated in the vertex shader.
//
// Shadows (a remaster addition; the original only had baked tile shadow
// planes, which it turns off on the software renderer): before the main
// pass flush() renders every opaque lit draw into a depth map from the key
// light (scene light 2, the warm sun) around the camera's focus, and lit
// pixels in shadow lose that light's diffuse and specular share.
//
// Needs a current OpenGL context (the SDL2 window or a hidden SDL2 window).

#include "renderer.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace rr4 {

struct GLSettings {
    int msaa = 4;            // multisample count of the scene buffer, 0 = off
    bool shadows = true;
    int shadowSize = 2048;   // shadow map texels per side
};

class GLRenderer final : public Renderer {
public:
    GLRenderer(int w, int h, const GLSettings &settings = {});
    ~GLRenderer() override;
    GLRenderer(const GLRenderer &) = delete;
    GLRenderer &operator=(const GLRenderer &) = delete;

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

    // Copy the finished frame to the default framebuffer, scaled to the
    // window size.
    void present(int windowW, int windowH);

private:
    enum class Kind { Mesh, Sprite, Rect };
    struct Cmd {
        Kind kind = Kind::Mesh;
        // Mesh
        const Mesh *mesh = nullptr;
        size_t first = 0, count = 0;
        Mat4 model, view, proj;
        Vec3 camPos, camTarget;
        const Texture *tex = nullptr;
        uint32_t tint = 0xffffffffu;
        bool unlit = false, depthWrite = true;
        int cull = 1;
        float alpha = 1.0f;
        SurfaceLighting surface;
        uint32_t fogColor = 0;
        float fogStart = 0, fogEnd = 1, du = 0, dv = 0;
        // Sprite / Rect (framebuffer pixels)
        float x = 0, y = 0, w = 0, h = 0;
        uint32_t color = 0;
        SpriteStyle style;
    };
    struct GpuMesh { unsigned vbo = 0, ibo = 0; size_t verts = 0, idx = 0; };
    struct GpuTexture { unsigned id = 0; uint32_t version = 0; };
    struct Program {
        unsigned id = 0;
        std::unordered_map<std::string, int> loc;
        int at(const char *name);   // cached uniform location
    };

    const GpuMesh &gpuMesh(const Mesh &m);
    unsigned gpuTexture(const Texture &t);
    void createTargets();
    bool renderShadowMap(Mat4 &lightVP);   // false when nothing to shadow
    void drawSky();
    void drawMeshCmd(const Cmd &c, const Mat4 &lightVP, bool shadowed);
    void drawScreenQuad(const Cmd &c);
    void resolve();

    GLSettings settings_;
    std::vector<Cmd> cmds_;
    bool clearPending_ = false;

    std::unordered_map<const void *, GpuMesh> meshes_;      // by verts.data()
    std::unordered_map<const void *, GpuTexture> textures_; // by px.data()
    unsigned white_ = 0;

    Program meshProg_, shadowProg_, screenProg_, skyProg_;
    unsigned quadVbo_ = 0;

    // Scene (multisampled) and resolved framebuffers; shadow depth map.
    unsigned sceneFbo_ = 0, sceneColor_ = 0, sceneDepth_ = 0;
    unsigned resolveFbo_ = 0, resolveColor_ = 0;
    unsigned shadowFbo_ = 0, shadowTex_ = 0;
    float anisotropy_ = 1.0f;
};

} // namespace rr4
