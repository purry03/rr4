#include "gl_renderer.hpp"

#ifdef __vita__
#include <vitaGL.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <stdexcept>

namespace rr4 {

namespace {

// Pixels of a depth-writing draw below this alpha are discarded (the
// software rasterizer only writes depth above alpha 128/256); blended draws
// keep everything above one step of 8-bit alpha.
constexpr float kDepthAlphaCut = 0.5f;
constexpr float kBlendAlphaCut = 1.0f / 256.0f;

// Shadow map: an orthographic box around the camera focus, looking along
// the key light. kShadowHalf is half its width in world units; casters up
// to kShadowReach along the light still land in it.
constexpr float kShadowHalf = 3000.0f;
constexpr float kShadowReach = 20000.0f;
constexpr int kKeyLight = 1;             // kSceneLights[1], the warm sun
constexpr float kShadowStrength = 0.85f; // share of the key light removed
constexpr float kShadowBias = 0.0008f;   // depth compare bias (0..1 depth)
constexpr float kShadowSlopeBias = 2.0f, kShadowUnitBias = 4.0f;   // glPolygonOffset
constexpr int kMaxLights = 4;
constexpr float kMaxAnisotropy = 8.0f;

#ifdef __vita__
// vitaGL: draw straight into the display buffer (multisampling is chosen at
// vglInit), upload level 0 and let the GPU build the mip chain, and compile
// the shaders without the shadow map (sampler2DShadow is not translated).
constexpr bool kDrawToBackbuffer = true;
constexpr bool kUploadMips = false;
constexpr bool kShadowMapSupported = false;
const char *kShaderHeader = "#version 120\n#define RR4_NO_SHADOW_MAP 1\n";
#else
constexpr bool kDrawToBackbuffer = false;
constexpr bool kUploadMips = true;
constexpr bool kShadowMapSupported = true;
const char *kShaderHeader = "#version 120\n";
#endif

const char *kMeshVS = R"(attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aUV;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uLightVP;
uniform vec3 uCamPos;
uniform vec2 uUVOffset;
uniform vec3 uBase;        // emissive + ambient lights * material ambient
uniform vec3 uDiffuse;     // white when textured (useDiffuseWithTexture FALSE)
uniform vec3 uSpecular;
uniform float uShininess;  // exponent
uniform int uHasSpec;
uniform int uUnlit;
uniform int uLightCount;
uniform int uKeyLight;
uniform float uReflect;
uniform vec3 uLightColor[4];
uniform vec3 uLightToward[4];
uniform float uFogStart;
uniform float uFogScale;
varying vec2 vUV;
varying vec2 vRefl;
varying float vFog;
varying vec3 vLit;
varying vec3 vLitKey;
varying vec3 vSpec;
varying vec3 vSpecKey;
varying vec4 vShadow;
void main() {
    vec4 world = uModel * vec4(aPos, 1.0);
    vec4 view = uView * world;
    gl_Position = uProj * view;
    vUV = aUV + uUVOffset;
    vFog = clamp((-view.z - uFogStart) * uFogScale, 0.0, 1.0);
    vShadow = uLightVP * world;
    vLit = vec3(1.0); vLitKey = vec3(0.0); vSpec = vec3(0.0); vSpecKey = vec3(0.0);
    vRefl = vec2(0.0);
    if (uUnlit == 0) {
        vec3 wn = normalize((uModel * vec4(aNormal, 0.0)).xyz);
        vec3 toEye = normalize(uCamPos - world.xyz);
        vLit = uBase;
        for (int i = 0; i < 4; ++i) {
            if (i >= uLightCount) break;
            float ndl = dot(wn, uLightToward[i]);
            if (ndl <= 0.0) continue;
            vec3 d = uLightColor[i] * uDiffuse * ndl;
            vec3 s = vec3(0.0);
            if (uHasSpec != 0) {
                float ndh = dot(wn, normalize(uLightToward[i] + toEye));
                if (ndh > 0.0) s = uLightColor[i] * uSpecular * pow(ndh, uShininess);
            }
            if (i == uKeyLight) { vLitKey += d; vSpecKey += s; }
            else { vLit += d; vSpec += s; }
        }
        if (uReflect > 0.0) {
            // #reflection: sphere map from the camera-space reflection vector.
            vec3 e = normalize(view.xyz);
            vec3 n = normalize((uView * (uModel * vec4(aNormal, 0.0))).xyz);
            vec3 rf = e - n * (2.0 * dot(n, e));
            float m = max(2.0 * sqrt(rf.x * rf.x + rf.y * rf.y + (rf.z + 1.0) * (rf.z + 1.0)), 1e-6);
            vRefl = vec2(rf.x / m + 0.5, 1.0 - (rf.y / m + 0.5));
        }
    }
}
)";

const char *kMeshFS = R"(uniform sampler2D uTex, uRefl;
#ifndef RR4_NO_SHADOW_MAP
uniform sampler2DShadow uShadowMap;
#endif
uniform vec4 uTint;          // rgb tint, a = draw alpha * tint alpha
uniform int uHasTex;
uniform int uHasSpec;
uniform int uUnlit;
uniform int uShadowOn;
uniform float uReflect;
uniform float uAlphaCut;
uniform float uShadowTexel;
uniform float uShadowStrength;
uniform float uShadowBias;
uniform vec3 uFogColor;
varying vec2 vUV;
varying vec2 vRefl;
varying float vFog;
varying vec3 vLit;
varying vec3 vLitKey;
varying vec3 vSpec;
varying vec3 vSpecKey;
varying vec4 vShadow;
float shadowFactor() {
#ifdef RR4_NO_SHADOW_MAP
    return 1.0;
#else
    if (uShadowOn == 0) return 1.0;
    vec3 c = vShadow.xyz / vShadow.w * 0.5 + 0.5;
    if (c.x <= 0.0 || c.x >= 1.0 || c.y <= 0.0 || c.y >= 1.0 || c.z >= 1.0) return 1.0;
    float s = 0.0;
    s += shadow2D(uShadowMap, vec3(c.xy + vec2(-0.5, -0.5) * uShadowTexel, c.z - uShadowBias)).r;
    s += shadow2D(uShadowMap, vec3(c.xy + vec2( 0.5, -0.5) * uShadowTexel, c.z - uShadowBias)).r;
    s += shadow2D(uShadowMap, vec3(c.xy + vec2(-0.5,  0.5) * uShadowTexel, c.z - uShadowBias)).r;
    s += shadow2D(uShadowMap, vec3(c.xy + vec2( 0.5,  0.5) * uShadowTexel, c.z - uShadowBias)).r;
    s *= 0.25;
    // Fade out towards the edge of the shadow box.
    vec2 e = min(c.xy, 1.0 - c.xy);
    float fade = clamp(min(e.x, e.y) * 20.0, 0.0, 1.0);
    return 1.0 - (1.0 - s) * uShadowStrength * fade;
#endif
}
void main() {
    vec4 t = uHasTex != 0 ? texture2D(uTex, vUV) : vec4(1.0);
    float a = uTint.a * t.a;
    if (a < uAlphaCut) discard;
    vec3 c;
    if (uUnlit != 0) {
        c = t.rgb * uTint.rgb;
    } else {
        float sh = shadowFactor();
        c = t.rgb * uTint.rgb * min(vLit + vLitKey * sh, vec3(1.0));
        if (uReflect > 0.0) c = mix(c, texture2D(uRefl, vRefl).rgb, uReflect);
        if (uHasSpec != 0) c = min(c + vSpec + vSpecKey * sh, vec3(1.0));
    }
    c = mix(c, uFogColor, vFog);
    gl_FragColor = vec4(c, a);
}
)";

const char *kShadowVS = R"(attribute vec3 aPos;
attribute vec2 aUV;
uniform mat4 uModel;
uniform mat4 uLightVP;
uniform vec2 uUVOffset;
varying vec2 vUV;
void main() {
    gl_Position = uLightVP * (uModel * vec4(aPos, 1.0));
    vUV = aUV + uUVOffset;
}
)";

const char *kShadowFS = R"(uniform sampler2D uTex;
uniform int uHasTex;
uniform float uAlpha;
uniform float uAlphaCut;
varying vec2 vUV;
void main() {
    float a = uAlpha * (uHasTex != 0 ? texture2D(uTex, vUV).a : 1.0);
    if (a < uAlphaCut) discard;
    gl_FragColor = vec4(1.0);
}
)";

// Screen quads: aPos is the unit quad, placed in framebuffer pixels.
const char *kScreenVS = R"(attribute vec3 aPos;
uniform vec4 uRect;      // x, y, w, h in pixels, y down
uniform vec2 uViewport;
uniform vec4 uUV;        // u0, v0, u1, v1
varying vec2 vUV;
void main() {
    vec2 p = uRect.xy + aPos.xy * uRect.zw;
    gl_Position = vec4(p.x / uViewport.x * 2.0 - 1.0, 1.0 - p.y / uViewport.y * 2.0, 0.0, 1.0);
    vUV = mix(uUV.xy, uUV.zw, aPos.xy);
}
)";

const char *kScreenFS = R"(uniform sampler2D uTex;
uniform vec4 uColor;
varying vec2 vUV;
void main() {
    gl_FragColor = texture2D(uTex, vUV) * uColor;
}
)";

// vUV.y runs 0 at the top row to 1 at the bottom (the quad spans the screen).
const char *kSkyFS = R"(uniform vec3 uTop, uHorizon;
uniform float uCurve;
varying vec2 vUV;
void main() {
    float t = pow(clamp(vUV.y, 0.0, 1.0), uCurve);
    gl_FragColor = vec4(mix(uTop, uHorizon, t), 1.0);
}
)";

enum Attrib : unsigned { kAttribPos = 0, kAttribNormal = 1, kAttribUV = 2 };

Vec3 rgb(uint32_t c) {
    return {float(c & 255) / 255.0f, float((c >> 8) & 255) / 255.0f, float((c >> 16) & 255) / 255.0f};
}

unsigned compile(GLenum type, const char *src) {
    unsigned s = glCreateShader(type);
    const char *parts[] = {kShaderHeader, src};
    glShaderSource(s, 2, parts, nullptr);
    glCompileShader(s);
    int ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("GL shader compile failed: ") + log);
    }
    return s;
}

unsigned link(const char *vs, const char *fs) {
    unsigned p = glCreateProgram();
    unsigned v = compile(GL_VERTEX_SHADER, vs), f = compile(GL_FRAGMENT_SHADER, fs);
    glAttachShader(p, v);
    glAttachShader(p, f);
    glBindAttribLocation(p, kAttribPos, "aPos");
    glBindAttribLocation(p, kAttribNormal, "aNormal");
    glBindAttribLocation(p, kAttribUV, "aUV");
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    int ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(p, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("GL program link failed: ") + log);
    }
    return p;
}

// Orthographic projection for the shadow box (OpenGL glOrtho).
Mat4 ortho(float l, float r, float b, float t, float n, float f) {
    Mat4 m = Mat4::identity();
    m.m[0] = 2.0f / (r - l);
    m.m[5] = 2.0f / (t - b);
    m.m[10] = -2.0f / (f - n);
    m.m[12] = -(r + l) / (r - l);
    m.m[13] = -(t + b) / (t - b);
    m.m[14] = -(f + n) / (f - n);
    return m;
}

} // namespace

int GLRenderer::Program::at(const char *name) {
    auto it = loc.find(name);
    if (it != loc.end()) return it->second;
    int l = glGetUniformLocation(id, name);
    loc.emplace(name, l);
    return l;
}

GLRenderer::GLRenderer(int w, int h, const GLSettings &settings)
    : Renderer(w, h), settings_(settings) {
    settings_.shadows = settings_.shadows && kShadowMapSupported;
    meshProg_.id = link(kMeshVS, kMeshFS);
    shadowProg_.id = link(kShadowVS, kShadowFS);
    screenProg_.id = link(kScreenVS, kScreenFS);
    skyProg_.id = link(kScreenVS, kSkyFS);

    const float quad[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0};
    glGenBuffers(1, &quadVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    const uint32_t whitePx = 0xffffffffu;
    glGenTextures(1, &white_);
    glBindTexture(GL_TEXTURE_2D, white_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whitePx);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy_);
    anisotropy_ = std::clamp(anisotropy_, 1.0f, kMaxAnisotropy);
    createTargets();
}

GLRenderer::~GLRenderer() {
    for (auto &[k, m] : meshes_) {
        glDeleteBuffers(1, &m.vbo);
        glDeleteBuffers(1, &m.ibo);
    }
    for (auto &[k, t] : textures_) glDeleteTextures(1, &t.id);
    glDeleteTextures(1, &white_);
    glDeleteBuffers(1, &quadVbo_);
    for (Program *p : {&meshProg_, &shadowProg_, &screenProg_, &skyProg_}) glDeleteProgram(p->id);
    glDeleteFramebuffers(1, &sceneFbo_);
    glDeleteRenderbuffers(1, &sceneColor_);
    glDeleteRenderbuffers(1, &sceneDepth_);
    glDeleteFramebuffers(1, &resolveFbo_);
    glDeleteRenderbuffers(1, &resolveColor_);
    if (shadowFbo_) glDeleteFramebuffers(1, &shadowFbo_);
    if (shadowTex_) glDeleteTextures(1, &shadowTex_);
}

void GLRenderer::createTargets() {
    if (kDrawToBackbuffer) return;
#ifndef __vita__
    int maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    const int samples = std::min(settings_.msaa, maxSamples);

    glGenFramebuffers(1, &sceneFbo_);
    glGenRenderbuffers(1, &sceneColor_);
    glGenRenderbuffers(1, &sceneDepth_);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneColor_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneDepth_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, sceneColor_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sceneDepth_);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("GL scene framebuffer incomplete");

    glGenFramebuffers(1, &resolveFbo_);
    glGenRenderbuffers(1, &resolveColor_);
    glBindRenderbuffer(GL_RENDERBUFFER, resolveColor_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, resolveFbo_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, resolveColor_);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("GL resolve framebuffer incomplete");

    if (settings_.shadows) {
        const int n = settings_.shadowSize;
        glGenTextures(1, &shadowTex_);
        glBindTexture(GL_TEXTURE_2D, shadowTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, n, n, 0, GL_DEPTH_COMPONENT,
                     GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glGenFramebuffers(1, &shadowFbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex_, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("GL shadow framebuffer incomplete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

const GLRenderer::GpuMesh &GLRenderer::gpuMesh(const Mesh &m) {
    // Meshes are built once and never edited after their first draw, so the
    // vertex buffer address identifies them.
    GpuMesh &g = meshes_[m.verts.data()];
    if (g.vbo && g.verts == m.verts.size() && g.idx == m.idx.size()) return g;
    if (!g.vbo) {
        glGenBuffers(1, &g.vbo);
        glGenBuffers(1, &g.ibo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(m.verts.size() * sizeof(Vertex)), m.verts.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(m.idx.size() * sizeof(uint32_t)), m.idx.data(),
                 GL_STATIC_DRAW);
    g.verts = m.verts.size();
    g.idx = m.idx.size();
    return g;
}

unsigned GLRenderer::gpuTexture(const Texture &t) {
    if (t.px.empty()) return white_;
    GpuTexture &g = textures_[t.px.data()];
    if (g.id && g.version == t.version) return g.id;
    if (!g.id) glGenTextures(1, &g.id);
    glBindTexture(GL_TEXTURE_2D, g.id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.w, t.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t.px.data());
    const bool mipped = !t.mips.empty();
    if (kUploadMips) {
        for (size_t l = 0; l < t.mips.size(); ++l)
            glTexImage2D(GL_TEXTURE_2D, GLint(l + 1), GL_RGBA, t.mips[l].w, t.mips[l].h, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, t.mips[l].px.data());
#ifndef __vita__
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(t.mips.size()));
#endif
    } else if (mipped) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    if (gSmoothTextures) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        mipped ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (mipped) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy_);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        mipped ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    g.version = t.version;
    return g.id;
}

void GLRenderer::clearSky(const Vec3 &, float) {
    cmds_.clear();
    clearPending_ = true;
}

void GLRenderer::drawMesh(const Camera &cam, const Mat4 &model, const Mesh &mesh,
                          const Texture *tex, uint32_t tint, bool unlit, int cull,
                          float alpha, bool depthWrite, size_t idxBegin, size_t idxCount) {
    if (mesh.idx.empty() || idxBegin >= mesh.idx.size()) return;
    if (alpha * float((tint >> 24) & 255) <= 0.0f) return;
    Cmd c;
    c.kind = Kind::Mesh;
    c.mesh = &mesh;
    c.first = idxBegin;
    c.count = std::min(idxCount, mesh.idx.size() - idxBegin);
    c.model = model;
    c.view = cam.view();
    c.proj = cam.projection(float(width) / float(height));
    c.camPos = cam.pos;
    c.camTarget = cam.target;
    c.tex = tex && !tex->px.empty() ? tex : nullptr;
    c.tint = tint;
    c.unlit = unlit;
    c.cull = cull;
    c.alpha = alpha;
    c.depthWrite = depthWrite;
    c.surface = surface_;
    c.fogColor = fogColor_;
    c.fogStart = fogStart_;
    c.fogEnd = fogEnd_;
    c.du = uOffset_;
    c.dv = vOffset_;
    cmds_.push_back(c);
}

void GLRenderer::drawShadow(const Camera &, const Vec3 &, float) {
    // The shadow map already shades the ground under the car. Without it,
    // fall back to nothing rather than a second, different shadow.
}

void GLRenderer::drawSprite(const Texture &tex, float x, float y, float w, float h,
                            float alpha, const SpriteStyle &style) {
    if (tex.px.empty() || w <= 0.0f || h <= 0.0f || alpha <= 0.0f) return;
    Cmd c;
    c.kind = Kind::Sprite;
    c.tex = &tex;
    c.x = x; c.y = y; c.w = w; c.h = h;
    c.alpha = alpha;
    c.style = style;
    cmds_.push_back(c);
}

void GLRenderer::drawRect(int x, int y, int w, int h, uint32_t color, float alpha) {
    if (w <= 0 || h <= 0 || alpha <= 0.0f) return;
    Cmd c;
    c.kind = Kind::Rect;
    c.x = float(x); c.y = float(y); c.w = float(w); c.h = float(h);
    c.color = color;
    c.alpha = alpha;
    cmds_.push_back(c);
}

bool GLRenderer::renderShadowMap(Mat4 &lightVP) {
    const Cmd *focusCmd = nullptr;
    for (const Cmd &c : cmds_)
        if (c.kind == Kind::Mesh) focusCmd = &c;
    if (!focusCmd || int(lights_.size()) <= kKeyLight) return false;

    // Light view looking along the key light; the box is centred on the
    // camera focus and snapped to whole texels so it does not shimmer.
    const Vec3 toward = lights_[size_t(kKeyLight)].toward.normalized();
    const Mat4 view = Mat4::lookAt(toward, Vec3(0, 0, 0), Vec3(0, 1, 0));
    const Vec3 f = view.transformPoint(focusCmd->camTarget);
    const float texel = 2.0f * kShadowHalf / float(settings_.shadowSize);
    const float cx = std::floor(f.x / texel) * texel, cy = std::floor(f.y / texel) * texel;
    const float depth = -f.z;
    lightVP = ortho(cx - kShadowHalf, cx + kShadowHalf, cy - kShadowHalf, cy + kShadowHalf,
                    depth - kShadowReach, depth + kShadowReach) * view;

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
    glViewport(0, 0, settings_.shadowSize, settings_.shadowSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(kShadowSlopeBias, kShadowUnitBias);

    Program &p = shadowProg_;
    glUseProgram(p.id);
    glUniformMatrix4fv(p.at("uLightVP"), 1, GL_FALSE, lightVP.m);
    glUniform1i(p.at("uTex"), 0);
    glUniform1f(p.at("uAlphaCut"), kDepthAlphaCut);
    glActiveTexture(GL_TEXTURE0);
    glEnableVertexAttribArray(kAttribPos);
    glEnableVertexAttribArray(kAttribUV);
    glDisableVertexAttribArray(kAttribNormal);
    for (const Cmd &c : cmds_) {
        // Casters: opaque, lit, depth-writing geometry (no sky, smoke or decals).
        if (c.kind != Kind::Mesh || c.unlit || !c.depthWrite) continue;
        const GpuMesh &g = gpuMesh(*c.mesh);
        glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.ibo);
        glVertexAttribPointer(kAttribPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, p)));
        glVertexAttribPointer(kAttribUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, u)));
        glUniformMatrix4fv(p.at("uModel"), 1, GL_FALSE, c.model.m);
        glUniform2f(p.at("uUVOffset"), c.du, c.dv);
        glUniform1f(p.at("uAlpha"), c.alpha * float((c.tint >> 24) & 255) / 255.0f);
        glUniform1i(p.at("uHasTex"), c.tex ? 1 : 0);
        glBindTexture(GL_TEXTURE_2D, c.tex ? gpuTexture(*c.tex) : white_);
        glDrawElements(GL_TRIANGLES, GLsizei(c.count), GL_UNSIGNED_INT,
                       reinterpret_cast<void *>(c.first * sizeof(uint32_t)));
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
    return true;
}

void GLRenderer::drawSky() {
    Program &p = skyProg_;
    glUseProgram(p.id);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glUniform4f(p.at("uRect"), 0.0f, 0.0f, float(width), float(height));
    glUniform2f(p.at("uViewport"), float(width), float(height));
    glUniform4f(p.at("uUV"), 0.0f, 0.0f, 1.0f, 1.0f);
    Vec3 top = rgb(kSkyTop), horizon = rgb(kSkyHorizon);
    glUniform3f(p.at("uTop"), top.x, top.y, top.z);
    glUniform3f(p.at("uHorizon"), horizon.x, horizon.y, horizon.z);
    glUniform1f(p.at("uCurve"), kSkyCurve);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);
    glEnableVertexAttribArray(kAttribPos);
    glDisableVertexAttribArray(kAttribNormal);
    glDisableVertexAttribArray(kAttribUV);
    glVertexAttribPointer(kAttribPos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLRenderer::drawMeshCmd(const Cmd &c, const Mat4 &lightVP, bool shadowed) {
    Program &p = meshProg_;
    const GpuMesh &g = gpuMesh(*c.mesh);
    glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.ibo);
    glVertexAttribPointer(kAttribPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, p)));
    glVertexAttribPointer(kAttribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, n)));
    glVertexAttribPointer(kAttribUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, u)));

    glUniformMatrix4fv(p.at("uModel"), 1, GL_FALSE, c.model.m);
    glUniformMatrix4fv(p.at("uView"), 1, GL_FALSE, c.view.m);
    glUniformMatrix4fv(p.at("uProj"), 1, GL_FALSE, c.proj.m);
    glUniformMatrix4fv(p.at("uLightVP"), 1, GL_FALSE, lightVP.m);
    glUniform3f(p.at("uCamPos"), c.camPos.x, c.camPos.y, c.camPos.z);
    glUniform2f(p.at("uUVOffset"), c.du, c.dv);

    // Same lighting inputs as SoftRenderer::drawMesh.
    const SurfaceLighting &sf = c.surface;
    Vec3 base = sf.emissive;
    for (const SceneLight &l : lights_)
        if (l.kind == 0)
            base += Vec3(l.color.x * sf.ambient.x, l.color.y * sf.ambient.y, l.color.z * sf.ambient.z);
    const Vec3 diffuse = c.tex ? Vec3(1, 1, 1) : sf.diffuse;
    const bool hasSpec = !c.unlit && (sf.specular.x + sf.specular.y + sf.specular.z) > 0.0f;
    const Texture *refl = sf.reflectTex && !sf.reflectTex->px.empty() ? sf.reflectTex : nullptr;
    const float reflect = (!c.unlit && refl) ? clampf(sf.reflect, 0, 1) : 0.0f;
    glUniform3f(p.at("uBase"), base.x, base.y, base.z);
    glUniform3f(p.at("uDiffuse"), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(p.at("uSpecular"), sf.specular.x, sf.specular.y, sf.specular.z);
    glUniform1f(p.at("uShininess"), std::max(1.0f, sf.shininess * 128.0f));
    glUniform1i(p.at("uHasSpec"), hasSpec ? 1 : 0);
    glUniform1i(p.at("uUnlit"), c.unlit ? 1 : 0);
    glUniform1f(p.at("uReflect"), reflect);

    float colors[kMaxLights * 3] = {}, toward[kMaxLights * 3] = {};
    int count = 0, key = -1;
    for (size_t i = 0; i < lights_.size() && count < kMaxLights; ++i) {
        const SceneLight &l = lights_[i];
        if (l.kind == 0) continue;
        if (int(i) == kKeyLight) key = count;
        colors[count * 3] = l.color.x; colors[count * 3 + 1] = l.color.y; colors[count * 3 + 2] = l.color.z;
        toward[count * 3] = l.toward.x; toward[count * 3 + 1] = l.toward.y; toward[count * 3 + 2] = l.toward.z;
        ++count;
    }
    glUniform3fv(p.at("uLightColor"), kMaxLights, colors);
    glUniform3fv(p.at("uLightToward"), kMaxLights, toward);
    glUniform1i(p.at("uLightCount"), count);
    glUniform1i(p.at("uKeyLight"), key);
    glUniform1f(p.at("uFogStart"), c.fogStart);
    glUniform1f(p.at("uFogScale"), 1.0f / std::max(1.0f, c.fogEnd - c.fogStart));
    Vec3 fog = rgb(c.fogColor);
    glUniform3f(p.at("uFogColor"), fog.x, fog.y, fog.z);

    Vec3 tint = rgb(c.tint);
    glUniform4f(p.at("uTint"), tint.x, tint.y, tint.z,
                clampf(c.alpha, 0, 1) * float((c.tint >> 24) & 255) / 255.0f);
    glUniform1i(p.at("uHasTex"), c.tex ? 1 : 0);
    glUniform1f(p.at("uAlphaCut"), c.depthWrite ? kDepthAlphaCut : kBlendAlphaCut);
    glUniform1i(p.at("uShadowOn"), shadowed && !c.unlit ? 1 : 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refl ? gpuTexture(*refl) : white_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, c.tex ? gpuTexture(*c.tex) : white_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // cull 1: front faces wind counter-clockwise on screen; cull 2: mirrored
    // geometry, front faces wind clockwise; cull 0: both sides.
    if (c.cull == 0) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        glCullFace(c.cull == 1 ? GL_BACK : GL_FRONT);
    }
    glDepthMask(c.depthWrite ? GL_TRUE : GL_FALSE);
    glDrawElements(GL_TRIANGLES, GLsizei(c.count), GL_UNSIGNED_INT,
                   reinterpret_cast<void *>(c.first * sizeof(uint32_t)));
}

void GLRenderer::drawScreenQuad(const Cmd &c) {
    Program &p = screenProg_;
    glUniform4f(p.at("uRect"), c.x, c.y, c.w, c.h);
    if (c.kind == Kind::Sprite) {
        glBindTexture(GL_TEXTURE_2D, gpuTexture(*c.tex));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        Vec3 tint = rgb(c.style.tint);
        glUniform4f(p.at("uColor"), tint.x, tint.y, tint.z, c.alpha);
        glUniform4f(p.at("uUV"), c.style.u0, c.style.v0, c.style.u1, c.style.v1);
    } else {
        glBindTexture(GL_TEXTURE_2D, white_);
        Vec3 col = rgb(c.color);
        glUniform4f(p.at("uColor"), col.x, col.y, col.z, c.alpha);
        glUniform4f(p.at("uUV"), 0.0f, 0.0f, 1.0f, 1.0f);
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLRenderer::flush() {
    if (cmds_.empty() && !clearPending_) return;

    Mat4 lightVP = Mat4::identity();
    bool shadowed = false;
    if (settings_.shadows) shadowed = renderShadowMap(lightVP);

    glBindFramebuffer(GL_FRAMEBUFFER, kDrawToBackbuffer ? 0 : sceneFbo_);
    glViewport(0, 0, width, height);
    if (clearPending_) {
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        drawSky();
        clearPending_ = false;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFrontFace(GL_CCW);

    bool meshState = false, screenState = false;
    for (const Cmd &c : cmds_) {
        if (c.kind == Kind::Mesh) {
            if (!meshState) {
                glUseProgram(meshProg_.id);
                glUniform1i(meshProg_.at("uTex"), 0);
                glUniform1i(meshProg_.at("uRefl"), 1);
                glUniform1i(meshProg_.at("uShadowMap"), 2);
                glUniform1f(meshProg_.at("uShadowTexel"), 1.0f / float(settings_.shadowSize));
                glUniform1f(meshProg_.at("uShadowStrength"), kShadowStrength);
                glUniform1f(meshProg_.at("uShadowBias"), kShadowBias);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, shadowTex_);
                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glEnableVertexAttribArray(kAttribPos);
                glEnableVertexAttribArray(kAttribNormal);
                glEnableVertexAttribArray(kAttribUV);
                meshState = true;
                screenState = false;
            }
            drawMeshCmd(c, lightVP, shadowed);
        } else {
            if (!screenState) {
                glUseProgram(screenProg_.id);
                glUniform1i(screenProg_.at("uTex"), 0);
                glUniform2f(screenProg_.at("uViewport"), float(width), float(height));
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);
                glEnableVertexAttribArray(kAttribPos);
                glDisableVertexAttribArray(kAttribNormal);
                glDisableVertexAttribArray(kAttribUV);
                glVertexAttribPointer(kAttribPos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
                glActiveTexture(GL_TEXTURE0);
                screenState = true;
                meshState = false;
            }
            drawScreenQuad(c);
        }
    }
    cmds_.clear();
    glDepthMask(GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderer::resolve() {
    if (kDrawToBackbuffer) return;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFbo_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFbo_);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void GLRenderer::readPixels(std::vector<uint32_t> &out) {
    flush();
    resolve();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, kDrawToBackbuffer ? 0 : resolveFbo_);
    std::vector<uint32_t> flipped(size_t(width) * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, flipped.data());
    out.resize(flipped.size());
    for (int y = 0; y < height; ++y)
        std::copy_n(flipped.begin() + size_t(height - 1 - y) * width, width,
                    out.begin() + size_t(y) * width);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderer::present(int windowW, int windowH) {
    flush();
    if (kDrawToBackbuffer) return;   // already on screen; the display swaps
    resolve();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, resolveFbo_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, windowW, windowH, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace rr4
