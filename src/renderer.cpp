#include "renderer.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>

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

void Renderer::clearSky(const Vec3 &, float) {
    uint32_t top = packRGBA(64, 122, 190);
    uint32_t horizon = packRGBA(196, 218, 238);
    for (int y = 0; y < height; ++y) {
        float t = float(y) / std::max(1, height - 1);
        t = std::pow(t, 0.75f);
        uint32_t c = mixColor(top, horizon, t);
        std::fill(color.begin() + size_t(y) * width, color.begin() + size_t(y + 1) * width, c);
    }
    std::fill(depth.begin(), depth.end(), 1e30f);
}

void Renderer::project(const ClipVert &v, float &sx, float &sy, float &sz) const {
    float invz = 1.0f / (-v.view.z);
    float f = 1.0f / std::tan(deg2rad(fov_) * 0.5f);
    float ndcx = v.view.x * f / aspect_ * invz;
    float ndcy = v.view.y * f * invz;
    sx = (ndcx * 0.5f + 0.5f) * width;
    sy = (1.0f - (ndcy * 0.5f + 0.5f)) * height;
    sz = -v.view.z;
}

void Renderer::rasterize(ClipVert a, ClipVert b, ClipVert c,
                         const Texture *tex, uint32_t tint, bool unlit, float alpha, bool depthWrite,
                         int cull) {
    float x0, y0, z0, x1, y1, z1, x2, y2, z2;
    project(a, x0, y0, z0);
    project(b, x1, y1, z1);
    project(c, x2, y2, z2);

    float area = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
    if (std::fabs(area) < 0.0001f) return;
    if (area > 0) {
        // cull 1: front faces are the negative-area winding (3DOM default).
        // cull 2: mirrored geometry, front faces use the positive winding.
        if (cull == 1) return;
        if (cull == 0) {
            std::swap(x1, x2); std::swap(y1, y2); std::swap(z1, z2);
            std::swap(b, c);
            area = -area;
        }
    } else if (cull == 2) {
        return;
    }
    float invArea = 1.0f / area;

    int minX = std::max(0, int(std::floor(std::min({x0, x1, x2}))));
    int maxX = std::min(width - 1, int(std::ceil(std::max({x0, x1, x2}))));
    int minY = std::max(0, int(std::floor(std::min({y0, y1, y2}))));
    int maxY = std::min(height - 1, int(std::ceil(std::max({y0, y1, y2}))));
    if (minX > maxX || minY > maxY) return;

    float iz0 = 1.0f / z0, iz1 = 1.0f / z1, iz2 = 1.0f / z2;
    int tr = tint & 255, tg = (tint >> 8) & 255, tb = (tint >> 16) & 255;
    int tA = (tint >> 24) & 255;

    for (int y = minY; y <= maxY; ++y) {
        float py = y + 0.5f;
        for (int x = minX; x <= maxX; ++x) {
            float px = x + 0.5f;
            float w0 = ((x1 - px) * (y2 - py) - (y1 - py) * (x2 - px)) * invArea;
            float w1 = ((x2 - px) * (y0 - py) - (y2 - py) * (x0 - px)) * invArea;
            float w2 = 1.0f - w0 - w1;
            if (w0 < -0.0001f || w1 < -0.0001f || w2 < -0.0001f) continue;

            float z = 1.0f / (w0 * iz0 + w1 * iz1 + w2 * iz2);
            size_t off = size_t(y) * width + x;
            if (z >= depth[off]) continue;

            float u = (a.u * w0 * iz0 + b.u * w1 * iz1 + c.u * w2 * iz2) * z;
            float v = (a.v * w0 * iz0 + b.v * w1 * iz1 + c.v * w2 * iz2) * z;
            float fog = (a.fog * w0 * iz0 + b.fog * w1 * iz1 + c.fog * w2 * iz2) * z;
            float shade = unlit ? 1.0f : (a.shade * w0 * iz0 + b.shade * w1 * iz1 + c.shade * w2 * iz2) * z;

            bool smoothTex = gSmoothTextures && z < 1600.0f;
            uint32_t texel = tex ? tex->sample(u, v, smoothTex) : packRGBA(255, 255, 255);
            int r = texel & 255, g = (texel >> 8) & 255, bb = (texel >> 16) & 255;
            int texA = (texel >> 24) & 255;
            r = int(r * (tr / 255.0f) * shade);
            g = int(g * (tg / 255.0f) * shade);
            bb = int(bb * (tb / 255.0f) * shade);
            uint32_t c = packRGBA(r, g, bb, texA);

            if (fog > 0.0f) c = mixColor(c, fogColor_, fog);

            float aOut = alpha * (tA / 255.0f) * (texA / 255.0f);
            if (aOut >= 0.999f) {
                color[off] = c | 0xff000000u;
            } else if (aOut > 0.003f) {
                uint32_t dst = color[off];
                int dr = dst & 255, dg = (dst >> 8) & 255, db = (dst >> 16) & 255;
                int sr = c & 255, sg = (c >> 8) & 255, sb = (c >> 16) & 255;
                int rr = int(sr * aOut + dr * (1 - aOut));
                int gg = int(sg * aOut + dg * (1 - aOut));
                int bbb = int(sb * aOut + db * (1 - aOut));
                color[off] = packRGBA(rr, gg, bbb);
            }
            if (depthWrite && aOut > 0.5f) depth[off] = z;
        }
    }
}

void Renderer::drawMesh(const Camera &cam, const Mat4 &model, const Mesh &mesh,
                        const Texture *tex, uint32_t tint, bool unlit, int cull,
                        float alpha, bool depthWrite, size_t idxBegin, size_t idxCount) {
    view_ = cam.view();
    aspect_ = float(width) / float(height);
    camPos_ = cam.pos;
    fov_ = cam.fovDeg;

    Vec3 sun = Vec3(0.45f, 0.82f, 0.35f).normalized();
    const float nearZ = cam.nearZ;

    size_t idxEnd = mesh.idx.size();
    if (idxBegin < idxEnd) idxEnd = std::min(idxEnd, idxBegin + idxCount);
    else idxEnd = idxBegin;
    for (size_t i = idxBegin; i + 2 < idxEnd; i += 3) {
        ClipVert cv[3];
        for (int k = 0; k < 3; ++k) {
            const Vertex &v = mesh.verts[mesh.idx[i + k]];
            Vec3 wp = model.transformPoint(v.p);
            Vec3 wn = model.transformVector(v.n).normalized();
            cv[k].view = view_.transformPoint(wp);
            cv[k].normal = wn;
            cv[k].u = v.u;
            cv[k].v = v.v;
            float dist = -cv[k].view.z;
            cv[k].fog = clampf((dist - fogStart_) / std::max(1.0f, fogEnd_ - fogStart_), 0, 1);
            float ndl = std::max(0.0f, wn.dot(sun));
            cv[k].shade = unlit ? 1.0f : (0.60f + 0.55f * ndl);
        }

        // near-plane clipping
        ClipVert poly[8];
        int n = 0;
        for (int k = 0; k < 3; ++k) {
            const ClipVert &cur = cv[k];
            const ClipVert &nxt = cv[(k + 1) % 3];
            bool curIn = cur.view.z <= -nearZ;
            bool nxtIn = nxt.view.z <= -nearZ;
            if (curIn) poly[n++] = cur;
            if (curIn != nxtIn) {
                float t = (-nearZ - cur.view.z) / (nxt.view.z - cur.view.z);
                ClipVert m;
                m.view = lerp(cur.view, nxt.view, t);
                m.normal = lerp(cur.normal, nxt.normal, t).normalized();
                m.u = cur.u + (nxt.u - cur.u) * t;
                m.v = cur.v + (nxt.v - cur.v) * t;
                m.fog = cur.fog + (nxt.fog - cur.fog) * t;
                m.shade = cur.shade + (nxt.shade - cur.shade) * t;
                poly[n++] = m;
            }
        }
        if (n < 3) continue;
        for (int k = 1; k + 1 < n; ++k) {
            rasterize(poly[0], poly[k], poly[k + 1], tex, tint, unlit, alpha, depthWrite, cull);
        }
    }
}

void Renderer::drawShadow(const Camera &cam, const Vec3 &pos, float radius) {
    static Texture shadowTex = makeShadowTexture();
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
    if (tex.px.empty() || w <= 0.0f || h <= 0.0f || alpha <= 0.0f) return;
    int x0 = std::max(0, int(std::floor(x)));
    int y0 = std::max(0, int(std::floor(y)));
    int x1 = std::min(width, int(std::ceil(x + w)));
    int y1 = std::min(height, int(std::ceil(y + h)));
    if (x1 <= x0 || y1 <= y0) return;

    for (int py = y0; py < y1; ++py) {
        float v = (float(py) + 0.5f - y) / h;
        int sy = std::clamp(int(v * float(tex.h)), 0, tex.h - 1);
        for (int px = x0; px < x1; ++px) {
            float u = (float(px) + 0.5f - x) / w;
            int sx = std::clamp(int(u * float(tex.w)), 0, tex.w - 1);
            uint32_t src = tex.px[size_t(sy) * tex.w + sx];
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
