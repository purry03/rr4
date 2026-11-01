#include "renderer.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>
#include <atomic>
#include <thread>

namespace rr4 {

static float frand(uint32_t &s) {
    s = s * 1664525u + 1013904223u;
    return float((s >> 8) & 0xffff) / 65535.0f;
}

Texture makeAsphaltTexture(int size) {
    Texture t; t.w = t.h = size; t.px.resize(size_t(size) * size);
    uint32_t seed = 0x1234abcd;
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            float n = frand(seed);
            int g = 66 + int(n * 22);
            // faint tar seams
            if (((x + y * 3) % 47) == 0) g -= 12;
            t.px[y * size + x] = packRGBA(g, g, g + 2);
        }
    return t;
}

Texture makeGrassTexture(int size) {
    Texture t; t.w = t.h = size; t.px.resize(size_t(size) * size);
    uint32_t seed = 0x9e3779b9;
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            float n = frand(seed);
            t.px[y * size + x] = packRGBA(int(40 + n * 26), int(78 + n * 38), int(35 + n * 20));
        }
    return t;
}

Texture makeConcreteTexture(int size) {
    Texture t; t.w = t.h = size; t.px.resize(size_t(size) * size);
    uint32_t seed = 0x51ed270b;
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            float n = frand(seed);
            int g = 128 + int(n * 34);
            if (x % 32 == 0 || y % 32 == 0) g -= 18;
            t.px[y * size + x] = packRGBA(g, g - 2, g - 6);
        }
    return t;
}

Texture makeWaterTexture(int size) {
    Texture t; t.w = t.h = size; t.px.resize(size_t(size) * size);
    uint32_t seed = 0xdeadbeef;
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            float n = frand(seed) * 0.25f;
            float wave = 0.5f + 0.5f * std::sin((x * 0.25f) + (y * 0.11f));
            t.px[y * size + x] = packRGBA(int(22 + 20 * wave + n * 20),
                                          int(66 + 34 * wave + n * 30),
                                          int(104 + 40 * wave + n * 30));
        }
    return t;
}

Texture makeCarTexture(int size) {
    Texture t; t.w = t.h = size; t.px.resize(size_t(size) * size);
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            // painted metal with a subtle vertical highlight
            float s = 0.82f + 0.18f * (1.0f - float(y) / size);
            int r = int(28 * s), g = int(180 * s), b = int(210 * s);
            if (y > size * 3 / 4) { r = 20; g = 22; b = 26; } // lower skirt
            t.px[y * size + x] = packRGBA(r, g, b);
        }
    return t;
}

// Concrete facade with a grid of windows; used for the city buildings.
Texture makeFacadeTexture(int size) {
    Texture t; t.w = t.h = size; t.px.resize(size_t(size) * size);
    uint32_t seed = 0x2f6e2b1u;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float n = frand(seed);
            int g = 142 + int(n * 30);
            uint32_t c = packRGBA(g, g - 3, g - 9);
            int wy = y % 24, wx = x % 24;
            if (wy >= 6 && wy <= 17 && wx >= 5 && wx <= 18) {
                float sky = 0.45f + 0.55f * (1.0f - float(wy - 6) / 11.0f);
                c = packRGBA(int(30 + 40 * sky), int(58 + 70 * sky), int(78 + 92 * sky));
            }
            t.px[y * size + x] = c;
        }
    }
    return t;
}

Texture makeShadowTexture(int size) {
    Texture t; t.w = t.h = size; t.px.resize(size_t(size) * size);
    float c = (size - 1) * 0.5f;
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            float dx = (x - c) / c, dy = (y - c) / c;
            float d = std::sqrt(dx * dx + dy * dy);
            float a = std::max(0.0f, 1.0f - d);
            a = a * a * 190.0f;
            t.px[y * size + x] = packRGBA(0, 0, 0, int(a));
        }
    return t;
}

Texture makeSkyGradient(int w, int h) {
    Texture t; t.w = w; t.h = h; t.px.resize(size_t(w) * h);
    for (int y = 0; y < h; ++y) {
        float f = float(y) / std::max(1, h - 1);
        uint32_t c = mixColor(packRGBA(70, 128, 196), packRGBA(205, 224, 240), f);
        for (int x = 0; x < w; ++x) t.px[y * w + x] = c;
    }
    return t;
}

static void pushTri(Mesh &m, const Vertex &a, const Vertex &b, const Vertex &c) {
    uint32_t base = uint32_t(m.verts.size());
    m.verts.push_back(a); m.verts.push_back(b); m.verts.push_back(c);
    m.idx.push_back(base); m.idx.push_back(base + 1); m.idx.push_back(base + 2);
}

void Mesh::addQuad(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d,
                   const Vec3 &n, float u0, float v0, float u1, float v1) {
    Vertex va{a, n, u0, v0}, vb{b, n, u1, v0}, vc{c, n, u1, v1}, vd{d, n, u0, v1};
    pushTri(*this, va, vb, vc);
    pushTri(*this, va, vc, vd);
}

void Mesh::addTri(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &n) {
    pushTri(*this, {a, n, 0, 0}, {b, n, 0, 0}, {c, n, 0, 0});
}

void Mesh::addBox(const Vec3 &center, const Vec3 &size, const Vec3 &uvScale) {
    Vec3 h = size * 0.5f;
    Vec3 p000 = center + Vec3(-h.x, -h.y, -h.z);
    Vec3 p111 = center + Vec3(h.x, h.y, h.z);
    float ux = uvScale.x, uy = uvScale.y, uz = uvScale.z;
    // +Z (front)
    addQuad({p000.x, p000.y, p111.z}, {p111.x, p000.y, p111.z},
            {p111.x, p111.y, p111.z}, {p000.x, p111.y, p111.z}, {0, 0, 1}, 0, 0, ux, uy);
    // -Z (rear)
    addQuad({p111.x, p000.y, p000.z}, {p000.x, p000.y, p000.z},
            {p000.x, p111.y, p000.z}, {p111.x, p111.y, p000.z}, {0, 0, -1}, 0, 0, ux, uy);
    // +X
    addQuad({p111.x, p000.y, p111.z}, {p111.x, p000.y, p000.z},
            {p111.x, p111.y, p000.z}, {p111.x, p111.y, p111.z}, {1, 0, 0}, 0, 0, uz, uy);
    // -X
    addQuad({p000.x, p000.y, p000.z}, {p000.x, p000.y, p111.z},
            {p000.x, p111.y, p111.z}, {p000.x, p111.y, p000.z}, {-1, 0, 0}, 0, 0, uz, uy);
    // +Y
    addQuad({p000.x, p111.y, p111.z}, {p111.x, p111.y, p111.z},
            {p111.x, p111.y, p000.z}, {p000.x, p111.y, p000.z}, {0, 1, 0}, 0, 0, ux, uz);
    // -Y
    addQuad({p000.x, p000.y, p000.z}, {p111.x, p000.y, p000.z},
            {p111.x, p000.y, p111.z}, {p000.x, p000.y, p111.z}, {0, -1, 0}, 0, 0, ux, uz);
}

void Mesh::addCylinderX(const Vec3 &center, float radius, float width, int sides, const Vec3 &uvScale) {
    float hw = width * 0.5f;
    for (int i = 0; i < sides; ++i) {
        float a0 = 2 * PI * i / sides, a1 = 2 * PI * (i + 1) / sides;
        Vec3 n0{0, std::cos(a0), std::sin(a0)}, n1{0, std::cos(a1), std::sin(a1)};
        Vec3 p0 = center + Vec3(-hw, n0.y * radius, n0.z * radius);
        Vec3 p1 = center + Vec3(-hw, n1.y * radius, n1.z * radius);
        Vec3 p2 = center + Vec3(hw, n1.y * radius, n1.z * radius);
        Vec3 p3 = center + Vec3(hw, n0.y * radius, n0.z * radius);
        float u0 = float(i) / sides, u1 = float(i + 1) / sides;
        Vertex a{p0, n0, u0, 0}, b{p1, n1, u1, 0}, c{p2, n1, u1, uvScale.y}, d{p3, n0, u0, uvScale.y};
        pushTri(*this, a, b, c); pushTri(*this, a, c, d);
    }
    // caps
    for (int side = -1; side <= 1; side += 2) {
        float x = center.x + side * hw;
        Vec3 n{float(side), 0, 0};
        for (int i = 0; i < sides; ++i) {
            float a0 = 2 * PI * i / sides, a1 = 2 * PI * (i + 1) / sides;
            Vec3 p0{x, center.y + std::cos(a0) * radius, center.z + std::sin(a0) * radius};
            Vec3 p1{x, center.y + std::cos(a1) * radius, center.z + std::sin(a1) * radius};
            pushTri(*this, {center + Vec3(side * hw, 0, 0), n, 0, 0}, {p0, n, 0, 0}, {p1, n, 0, 0});
        }
    }
}

Mat4 Camera::projection(float aspect) const {
    // Camera looks down -Z in view space. We use a simple perspective matrix.
    float f = 1.0f / std::tan(deg2rad(fovDeg) * 0.5f);
    Mat4 r{};
    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (farZ + nearZ) / (nearZ - farZ);
    r.m[11] = -1.0f;
    r.m[14] = (2 * farZ * nearZ) / (nearZ - farZ);
    return r;
}

void Texture::buildMips() {
    mips.clear();
    while (true) {
        const Texture &src = mips.empty() ? *this : mips.back();
        if (src.w <= 1 && src.h <= 1) break;
        Texture m;
        m.w = std::max(1, src.w / 2);
        m.h = std::max(1, src.h / 2);
        m.px.resize(size_t(m.w) * m.h);
        int fx = src.w / m.w, fy = src.h / m.h;   // 1 or 2 (odd edge texel dropped)
        for (int y = 0; y < m.h; ++y)
            for (int x = 0; x < m.w; ++x) {
                // Alpha-weighted box filter so transparent texels do not
                // darken the edges of cut-out textures.
                int r = 0, g = 0, b = 0, a = 0, n = 0;
                for (int j = 0; j < fy; ++j)
                    for (int i = 0; i < fx; ++i) {
                        uint32_t c = src.px[size_t(y * fy + j) * src.w + x * fx + i];
                        int ca = int(c >> 24);
                        r += int(c & 255) * ca; g += int((c >> 8) & 255) * ca;
                        b += int((c >> 16) & 255) * ca; a += ca; ++n;
                    }
                m.px[size_t(y) * m.w + x] = a > 0 ? packRGBA(r / a, g / a, b / a, a / n) : 0u;
            }
        mips.push_back(std::move(m));
    }
}

Renderer::Renderer(int w, int h)
    : width(w), height(h), color(size_t(w) * h), depth(size_t(w) * h, 1e30f) {}

static constexpr int kStripRows = 8;

void Renderer::clearSky(const Vec3 &, float) {
    // Deferred: each strip clears its own rows at the start of flush().
    tris_.clear();
    states_.clear();
    uint32_t top = packRGBA(64, 122, 190);
    uint32_t horizon = packRGBA(196, 218, 238);
    skyRow_.resize(size_t(height));
    for (int y = 0; y < height; ++y) {
        float t = float(y) / std::max(1, height - 1);
        t = std::pow(t, 0.75f);
        skyRow_[size_t(y)] = mixColor(top, horizon, t);
    }
    clearPending_ = true;
}

void Renderer::setupTri(const ClipVert &a, const ClipVert &b, const ClipVert &c, int cull) {
    const ClipVert *v[3] = {&a, &b, &c};
    float sx[3], sy[3], iz[3];
    for (int k = 0; k < 3; ++k) {
        float d = -v[k]->view.z;
        iz[k] = 1.0f / d;
        sx[k] = (v[k]->view.x * projX_ * iz[k] * 0.5f + 0.5f) * float(width);
        sy[k] = (0.5f - v[k]->view.y * projY_ * iz[k] * 0.5f) * float(height);
    }

    float area = (sx[1] - sx[0]) * (sy[2] - sy[0]) - (sy[1] - sy[0]) * (sx[2] - sx[0]);
    if (std::fabs(area) < 0.0001f) return;
    if (area > 0) {
        // cull 1: front faces are the negative-area winding (3DOM default).
        // cull 2: mirrored geometry, front faces use the positive winding.
        if (cull == 1) return;
        if (cull == 0) {
            std::swap(sx[1], sx[2]); std::swap(sy[1], sy[2]); std::swap(iz[1], iz[2]);
            std::swap(v[1], v[2]);
            area = -area;
        }
    } else if (cull == 2) {
        return;
    }

    RasterTri t;
    t.minX = std::max(0, int(std::floor(std::min({sx[0], sx[1], sx[2]}))));
    t.maxX = std::min(width - 1, int(std::ceil(std::max({sx[0], sx[1], sx[2]}))));
    t.minY = std::max(0, int(std::floor(std::min({sy[0], sy[1], sy[2]}))));
    t.maxY = std::min(height - 1, int(std::ceil(std::max({sy[0], sy[1], sy[2]}))));
    if (t.minX > t.maxX || t.minY > t.maxY) return;

    // Barycentric k is the edge function of the edge opposite vertex k:
    // w_k(x, y) = ((x_i - x)(y_j - y) - (y_i - y)(x_j - x)) / area.
    float inv = 1.0f / area;
    for (int k = 0; k < 3; ++k) {
        int i = (k + 1) % 3, j = (k + 2) % 3;
        t.ex[k] = (sy[i] - sy[j]) * inv;
        t.ey[k] = (sx[j] - sx[i]) * inv;
        t.ec[k] = (sx[i] * sy[j] - sy[i] * sx[j]) * inv;
    }
    auto plane = [&](float a0, float a1, float a2) {
        Plane p;
        p.dx = a0 * t.ex[0] + a1 * t.ex[1] + a2 * t.ex[2];
        p.dy = a0 * t.ey[0] + a1 * t.ey[1] + a2 * t.ey[2];
        p.c  = a0 * t.ec[0] + a1 * t.ec[1] + a2 * t.ec[2];
        return p;
    };
    t.iz = plane(iz[0], iz[1], iz[2]);
    t.uz = plane(v[0]->u * iz[0], v[1]->u * iz[1], v[2]->u * iz[2]);
    t.vz = plane(v[0]->v * iz[0], v[1]->v * iz[1], v[2]->v * iz[2]);
    t.fz = plane(v[0]->fog * iz[0], v[1]->fog * iz[1], v[2]->fog * iz[2]);
    const DrawState &st = states_.back();
    auto vplane = [&](float ClipVert::*m) { return plane(v[0]->*m * iz[0], v[1]->*m * iz[1], v[2]->*m * iz[2]); };
    auto cplane = [&](Vec3 ClipVert::*m, float Vec3::*c) {
        return plane((v[0]->*m).*c * iz[0], (v[1]->*m).*c * iz[1], (v[2]->*m).*c * iz[2]);
    };
    if (!st.unlit) {
        t.lr = cplane(&ClipVert::lit, &Vec3::x);
        t.lg = cplane(&ClipVert::lit, &Vec3::y);
        t.lb = cplane(&ClipVert::lit, &Vec3::z);
    }
    if (st.hasSpec) {
        t.sr = cplane(&ClipVert::spec, &Vec3::x);
        t.sg = cplane(&ClipVert::spec, &Vec3::y);
        t.sb = cplane(&ClipVert::spec, &Vec3::z);
    }
    if (st.reflect256 > 0) {
        t.ru = vplane(&ClipVert::ru);
        t.rv = vplane(&ClipVert::rv);
    }
    t.state = uint32_t(states_.size() - 1);
    tris_.push_back(t);
}

void Renderer::rasterStrip(const RasterTri &t, int yBegin, int yEnd) {
    const DrawState &st = states_[t.state];
    const Texture *tex = st.tex && !st.tex->px.empty() ? st.tex : nullptr;
    const float texW = tex ? float(tex->w) : 0.0f, texH = tex ? float(tex->h) : 0.0f;
    const int maxLevel = tex ? tex->levels() - 1 : 0;
    const int fr = (st.fogColor & 255), fg = (st.fogColor >> 8) & 255, fb = (st.fogColor >> 16) & 255;
    constexpr float eps = -0.0001f;

    for (int y = std::max(yBegin, t.minY); y <= std::min(yEnd - 1, t.maxY); ++y) {
        const float py = float(y) + 0.5f;
        // Span of pixel centres inside all three edges on this row.
        float xl = float(t.minX) + 0.5f, xr = float(t.maxX) + 0.5f;
        bool empty = false;
        for (int k = 0; k < 3; ++k) {
            float rowC = t.ey[k] * py + t.ec[k];
            if (t.ex[k] > 0.0f)      xl = std::max(xl, (eps - rowC) / t.ex[k]);
            else if (t.ex[k] < 0.0f) xr = std::min(xr, (eps - rowC) / t.ex[k]);
            else if (rowC < eps) { empty = true; break; }
        }
        if (empty) continue;
        int x0 = std::max(t.minX, int(std::ceil(xl - 0.5f)));
        int x1 = std::min(t.maxX, int(std::floor(xr - 0.5f)));
        if (x0 > x1) continue;

        float px = float(x0) + 0.5f;
        float izv = t.iz.c + t.iz.dx * px + t.iz.dy * py;
        float uzv = t.uz.c + t.uz.dx * px + t.uz.dy * py;
        float vzv = t.vz.c + t.vz.dx * px + t.vz.dy * py;
        float fzv = t.fz.c + t.fz.dx * px + t.fz.dy * py;
        auto at = [&](const Plane &p) { return p.c + p.dx * px + p.dy * py; };
        float lrv = 0, lgv = 0, lbv = 0, srv = 0, sgv = 0, sbv = 0, ruv = 0, rvv = 0;
        if (!st.unlit) { lrv = at(t.lr); lgv = at(t.lg); lbv = at(t.lb); }
        if (st.hasSpec) { srv = at(t.sr); sgv = at(t.sg); sbv = at(t.sb); }
        if (st.reflect256 > 0) { ruv = at(t.ru); rvv = at(t.rv); }
        size_t off = size_t(y) * width + x0;

        for (int x = x0; x <= x1; ++x, ++off,
                 izv += t.iz.dx, uzv += t.uz.dx, vzv += t.vz.dx, fzv += t.fz.dx,
                 lrv += t.lr.dx, lgv += t.lg.dx, lbv += t.lb.dx,
                 srv += t.sr.dx, sgv += t.sg.dx, sbv += t.sb.dx,
                 ruv += t.ru.dx, rvv += t.rv.dx) {
            if (izv <= 0.0f) continue;
            float z = 1.0f / izv;
            if (z >= depth[off]) continue;

            uint32_t texel = 0xffffffffu;
            if (tex) {
                float u = uzv * z, v = vzv * z;
                int lvl = 0;
                if (maxLevel > 0) {
                    // Exact screen derivatives of the perspective-correct UV:
                    // d(U/W)/dx = (dU/dx - u dW/dx) / W.
                    float dudx = (t.uz.dx - u * t.iz.dx) * z * texW;
                    float dvdx = (t.vz.dx - v * t.iz.dx) * z * texH;
                    float dudy = (t.uz.dy - u * t.iz.dy) * z * texW;
                    float dvdy = (t.vz.dy - v * t.iz.dy) * z * texH;
                    // Isotropic LOD from the longer axis blurs road surfaces
                    // seen at grazing angles; blend toward the geometric mean
                    // of both axes (2 * texel area), never above the longer one.
                    float rho2 = std::min(std::max(dudx * dudx + dvdx * dvdx, dudy * dudy + dvdy * dvdy),
                                          2.0f * std::fabs(dudx * dvdy - dvdx * dudy));
                    // floor(log2(rho)) from the float exponent of rho^2.
                    uint32_t bits; std::memcpy(&bits, &rho2, 4);
                    int e = int((bits >> 23) & 255) - 127;
                    lvl = std::clamp(e >> 1, 0, maxLevel);
                }
                texel = tex->level(lvl).sample(u, v, gSmoothTextures);
            }

            int texA = int(texel >> 24);
            int a = (st.alpha256 * texA) >> 8;   // 0..256
            if (a <= 0) continue;

            int r, g, b;
            if (st.unlit) {
                r = (int(texel & 255) * st.tr) >> 8;
                g = (int((texel >> 8) & 255) * st.tg) >> 8;
                b = (int((texel >> 16) & 255) * st.tb) >> 8;
            } else {
                // Lit colour is clamped to 1 before it modulates the texture.
                int l0 = std::min(256, int(lrv * z * 256.0f));
                int l1 = std::min(256, int(lgv * z * 256.0f));
                int l2 = std::min(256, int(lbv * z * 256.0f));
                r = (int(texel & 255) * st.tr * l0) >> 16;
                g = (int((texel >> 8) & 255) * st.tg * l1) >> 16;
                b = (int((texel >> 16) & 255) * st.tb * l2) >> 16;
            }
            if (st.reflect256 > 0) {
                // Texture layer 2: #reflection mode, #blend by a constant.
                uint32_t rt = st.reflectTex->sample(ruv * z, rvv * z, true);
                int k = st.reflect256;
                r += (((int(rt & 255)) - r) * k) >> 8;
                g += (((int((rt >> 8) & 255)) - g) * k) >> 8;
                b += (((int((rt >> 16) & 255)) - b) * k) >> 8;
            }
            if (st.hasSpec) {
                r = std::min(255, r + int(srv * z * 255.0f));
                g = std::min(255, g + int(sgv * z * 255.0f));
                b = std::min(255, b + int(sbv * z * 255.0f));
            }

            float fog = fzv * z;
            if (fog > 0.0f) {
                int f = std::min(256, int(fog * 256.0f));
                r += ((fr - r) * f) >> 8;
                g += ((fg - g) * f) >> 8;
                b += ((fb - b) * f) >> 8;
            }

            if (a >= 255) {
                color[off] = uint32_t(r) | (uint32_t(g) << 8) | (uint32_t(b) << 16) | 0xff000000u;
            } else {
                uint32_t dst = color[off];
                int dr = dst & 255, dg = (dst >> 8) & 255, db = (dst >> 16) & 255;
                r = dr + (((r - dr) * a) >> 8);
                g = dg + (((g - dg) * a) >> 8);
                b = db + (((b - db) * a) >> 8);
                color[off] = uint32_t(r) | (uint32_t(g) << 8) | (uint32_t(b) << 16) | 0xff000000u;
            }
            if (st.depthWrite && a > 128) depth[off] = z;
        }
    }
}

void Renderer::flush() {
    if (tris_.empty() && !clearPending_) return;
    const int nStrips = (height + kStripRows - 1) / kStripRows;
    bins_.resize(size_t(nStrips));
    for (auto &b : bins_) b.clear();
    for (uint32_t i = 0; i < tris_.size(); ++i) {
        const RasterTri &t = tris_[i];
        for (int s = t.minY / kStripRows; s <= t.maxY / kStripRows; ++s) bins_[size_t(s)].push_back(i);
    }

    std::atomic<int> next{0};
    auto work = [&] {
        for (int s; (s = next.fetch_add(1)) < nStrips;) {
            int y0 = s * kStripRows, y1 = std::min(height, y0 + kStripRows);
            if (clearPending_) {
                for (int y = y0; y < y1; ++y)
                    std::fill_n(color.begin() + size_t(y) * width, width, skyRow_[size_t(y)]);
                std::fill(depth.begin() + size_t(y0) * width, depth.begin() + size_t(y1) * width, 1e30f);
            }
            for (uint32_t i : bins_[size_t(s)]) rasterStrip(tris_[i], y0, y1);
        }
    };
    unsigned n = std::max(1u, std::thread::hardware_concurrency());
    {
        std::vector<std::jthread> pool;
        pool.reserve(n - 1);
        for (unsigned i = 1; i < n; ++i) pool.emplace_back(work);
        work();
    }
    tris_.clear();
    states_.clear();
    clearPending_ = false;
}

void Renderer::drawMesh(const Camera &cam, const Mat4 &model, const Mesh &mesh,
                        const Texture *tex, uint32_t tint, bool unlit, int cull,
                        float alpha, bool depthWrite, size_t idxBegin, size_t idxCount) {
    const Mat4 view = cam.view();
    const Mat4 mv = view * model;
    const float aspect = float(width) / float(height);
    const float f = 1.0f / std::tan(deg2rad(cam.fovDeg) * 0.5f);
    projX_ = f / aspect;
    projY_ = f;

    DrawState st;
    st.tex = tex;
    // 0..255 mapped to 0..256 so a white tint is an exact identity multiply.
    auto ch = [&](int sh) { int c = int((tint >> sh) & 255); return c + (c >> 7); };
    st.tr = ch(0); st.tg = ch(8); st.tb = ch(16);
    st.alpha256 = int(clampf(alpha, 0, 1) * float((tint >> 24) & 255) * (256.0f / 255.0f) + 0.5f);
    st.fogColor = fogColor_;
    st.unlit = unlit;
    st.depthWrite = depthWrite;
    const SurfaceLighting &sf = surface_;
    st.hasSpec = !unlit && (sf.specular.x + sf.specular.y + sf.specular.z) > 0.0f;
    st.reflectTex = sf.reflectTex && !sf.reflectTex->px.empty() ? sf.reflectTex : nullptr;
    st.reflect256 = (!unlit && st.reflectTex) ? int(clampf(sf.reflect, 0, 1) * 256.0f) : 0;
    // Diffuse is ignored when the shader is textured (useDiffuseWithTexture).
    const Vec3 diffuse = tex ? Vec3(1, 1, 1) : sf.diffuse;
    const float shinyExp = std::max(1.0f, sf.shininess * 128.0f);
    if (st.alpha256 <= 0) return;
    states_.push_back(st);
    const size_t trisBefore = tris_.size();

    const float nearZ = cam.nearZ;
    const float fogScale = 1.0f / std::max(1.0f, fogEnd_ - fogStart_);

    if (vstamp_.size() < mesh.verts.size()) {
        vstamp_.resize(mesh.verts.size(), 0);
        vcache_.resize(mesh.verts.size());
    }
    if (++stamp_ == 0) { std::fill(vstamp_.begin(), vstamp_.end(), 0); stamp_ = 1; }
    auto vert = [&](uint32_t vi) -> const ClipVert & {
        ClipVert &cv = vcache_[vi];
        if (vstamp_[vi] == stamp_) return cv;
        vstamp_[vi] = stamp_;
        const Vertex &v = mesh.verts[vi];
        cv.view = mv.transformPoint(v.p);
        cv.u = v.u + uOffset_;
        cv.v = v.v + vOffset_;
        cv.fog = clampf((-cv.view.z - fogStart_) * fogScale, 0, 1);
        cv.lit = {1, 1, 1};
        cv.spec = {};
        cv.ru = cv.rv = 0.0f;
        if (!unlit) {
            Vec3 wn = model.transformVector(v.n).normalized();
            Vec3 lit = sf.emissive;
            Vec3 spec{};
            Vec3 toEye{};
            if (st.hasSpec) toEye = (cam.pos - model.transformPoint(v.p)).normalized();
            for (const SceneLight &l : lights_) {
                if (l.kind == 0) {
                    lit += Vec3(l.color.x * sf.ambient.x, l.color.y * sf.ambient.y,
                                l.color.z * sf.ambient.z);
                    continue;
                }
                float ndl = wn.dot(l.toward);
                if (ndl <= 0.0f) continue;
                lit += Vec3(l.color.x * diffuse.x, l.color.y * diffuse.y, l.color.z * diffuse.z) * ndl;
                if (st.hasSpec) {
                    float ndh = wn.dot((l.toward + toEye).normalized());
                    if (ndh > 0.0f) {
                        float s = std::pow(ndh, shinyExp);
                        spec += Vec3(l.color.x * sf.specular.x, l.color.y * sf.specular.y,
                                     l.color.z * sf.specular.z) * s;
                    }
                }
            }
            cv.lit = lit;
            cv.spec = spec;
            if (st.reflect256 > 0) {
                // #reflection: sphere map from the camera-space reflection
                // vector (OpenGL GL_SPHERE_MAP); V flipped like mesh UVs, so
                // surfaces facing up pick the sky rows of the map.
                Vec3 e = cv.view.normalized();
                Vec3 n = mv.transformVector(v.n).normalized();
                Vec3 rf = e - n * (2.0f * n.dot(e));
                float m = 2.0f * std::sqrt(rf.x * rf.x + rf.y * rf.y + (rf.z + 1.0f) * (rf.z + 1.0f));
                if (m < 1e-6f) m = 1e-6f;
                cv.ru = rf.x / m + 0.5f;
                cv.rv = 1.0f - (rf.y / m + 0.5f);
            }
        }
        return cv;
    };
    // Frustum outcode: left, right, bottom, top, near.
    auto outcode = [&](const Vec3 &p) {
        float d = -p.z;
        float x = p.x * projX_, y = p.y * projY_;
        return int(x < -d) | (int(x > d) << 1) | (int(y < -d) << 2) | (int(y > d) << 3) |
               (int(d < nearZ) << 4);
    };

    size_t idxEnd = mesh.idx.size();
    if (idxBegin < idxEnd) idxEnd = std::min(idxEnd, idxBegin + idxCount);
    else idxEnd = idxBegin;
    for (size_t i = idxBegin; i + 2 < idxEnd; i += 3) {
        const ClipVert *cv[3] = {&vert(mesh.idx[i]), &vert(mesh.idx[i + 1]), &vert(mesh.idx[i + 2])};
        int o0 = outcode(cv[0]->view), o1 = outcode(cv[1]->view), o2 = outcode(cv[2]->view);
        if (o0 & o1 & o2) continue;              // entirely outside one plane
        if (!((o0 | o1 | o2) & 16)) {            // fully in front of the near plane
            setupTri(*cv[0], *cv[1], *cv[2], cull);
            continue;
        }

        // near-plane clipping
        ClipVert poly[4];
        int n = 0;
        for (int k = 0; k < 3; ++k) {
            const ClipVert &cur = *cv[k];
            const ClipVert &nxt = *cv[(k + 1) % 3];
            bool curIn = cur.view.z <= -nearZ;
            bool nxtIn = nxt.view.z <= -nearZ;
            if (curIn) poly[n++] = cur;
            if (curIn != nxtIn) {
                float t = (-nearZ - cur.view.z) / (nxt.view.z - cur.view.z);
                ClipVert m;
                m.view = lerp(cur.view, nxt.view, t);
                m.u = cur.u + (nxt.u - cur.u) * t;
                m.v = cur.v + (nxt.v - cur.v) * t;
                m.fog = cur.fog + (nxt.fog - cur.fog) * t;
                m.lit = lerp(cur.lit, nxt.lit, t);
                m.spec = lerp(cur.spec, nxt.spec, t);
                m.ru = cur.ru + (nxt.ru - cur.ru) * t;
                m.rv = cur.rv + (nxt.rv - cur.rv) * t;
                poly[n++] = m;
            }
        }
        for (int k = 1; k + 1 < n; ++k) setupTri(poly[0], poly[k], poly[k + 1], cull);
    }
    if (tris_.size() == trisBefore) states_.pop_back();
}

void Renderer::drawShadow(const Camera &cam, const Vec3 &pos, float radius) {
    static Texture shadowTex = [] { Texture t = makeShadowTexture(); t.buildMips(); return t; }();
    Mesh m;
    Vec3 ground{pos.x, 1.0f, pos.z};
    Vec3 n{0, 1, 0};
    m.addQuad({ground.x - radius, ground.y, ground.z - radius * 1.7f},
              {ground.x + radius, ground.y, ground.z - radius * 1.7f},
              {ground.x + radius, ground.y, ground.z + radius * 1.7f},
              {ground.x - radius, ground.y, ground.z + radius * 1.7f}, n);
    drawMesh(cam, Mat4::identity(), m, &shadowTex, packRGBA(0, 0, 0), true, false,
             0.85f, false);
}

void Renderer::drawSprite(const Texture &tex, float x, float y, float w, float h,
                          float alpha) {
    flush();
    if (tex.px.empty() || w <= 0.0f || h <= 0.0f || alpha <= 0.0f) return;
    int x0 = std::max(0, int(std::floor(x)));
    int y0 = std::max(0, int(std::floor(y)));
    int x1 = std::min(width, int(std::ceil(x + w)));
    int y1 = std::min(height, int(std::ceil(y + h)));
    if (x1 <= x0 || y1 <= y0) return;

    for (int py = y0; py < y1; ++py) {
        float v = (float(py) + 0.5f - y) / h;
        for (int px = x0; px < x1; ++px) {
            float u = (float(px) + 0.5f - x) / w;
            uint32_t src = tex.sampleClamped(u, v);
            float a = float((src >> 24) & 255) / 255.0f * alpha;
            if (a <= 0.0f) continue;
            uint32_t &dst = color[size_t(py) * width + px];
            int sr = src & 255, sg = (src >> 8) & 255, sb = (src >> 16) & 255;
            int dr = dst & 255, dg = (dst >> 8) & 255, db = (dst >> 16) & 255;
            int r = int(float(sr) * a + float(dr) * (1.0f - a));
            int g = int(float(sg) * a + float(dg) * (1.0f - a));
            int b = int(float(sb) * a + float(db) * (1.0f - a));
            dst = packRGBA(r, g, b, 255);
        }
    }
}

} // namespace rr4
