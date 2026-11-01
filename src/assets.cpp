#include "assets.hpp"

#include <SFML/Graphics/Image.hpp>

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <fstream>

namespace rr4 {

W3DAssets gAssets;

void AssetMesh::computeBounds() {
    min = {1e30f, 1e30f, 1e30f};
    max = {-1e30f, -1e30f, -1e30f};
    for (const Vertex &v : verts) {
        min.x = std::min(min.x, v.p.x); min.y = std::min(min.y, v.p.y); min.z = std::min(min.z, v.p.z);
        max.x = std::max(max.x, v.p.x); max.y = std::max(max.y, v.p.y); max.z = std::max(max.z, v.p.z);
    }
}

namespace {

struct Reader {
    const uint8_t *p = nullptr;
    size_t n = 0, o = 0;
    bool ok = true;

    bool need(size_t k) {
        if (!ok || o + k > n) { ok = false; return false; }
        return true;
    }
    uint32_t u32() {
        if (!need(4)) return 0;
        uint32_t v = uint32_t(p[o]) | (uint32_t(p[o + 1]) << 8) |
                     (uint32_t(p[o + 2]) << 16) | (uint32_t(p[o + 3]) << 24);
        o += 4; return v;
    }
    int32_t i32() { return int32_t(u32()); }
    float f32() {
        uint32_t v = u32();
        float f; std::memcpy(&f, &v, 4); return f;
    }
    uint8_t u8() { if (!need(1)) return 0; return p[o++]; }
    std::string str() {
        uint32_t len = u32();
        if (!need(len)) return {};
        std::string s(reinterpret_cast<const char *>(p + o), len);
        o += len;
        return s;
    }
};

Texture makeFallbackTexture() {
    Texture t; t.w = t.h = 2;
    t.px = {packRGBA(255, 0, 255), packRGBA(0, 0, 0),
            packRGBA(0, 0, 0), packRGBA(255, 0, 255)};
    return t;
}

} // namespace

bool W3DAssets::load(const std::string &path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        std::fprintf(stderr, "assets: cannot open %s\n", path.c_str());
        return false;
    }
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());
    Reader r{data.data(), data.size(), 0, true};
    if (r.u32() != 0x57345252u /* "RR4W" LE */) { // 'R','R','4','W'
        std::fprintf(stderr, "assets: bad magic in %s\n", path.c_str());
        return false;
    }
    uint32_t version = r.u32();
    uint32_t texCount = r.u32();
    uint32_t matCount = r.u32();
    uint32_t meshCount = r.u32();
    if (version != 1 || !r.ok) {
        std::fprintf(stderr, "assets: unsupported version %u\n", version);
        return false;
    }

    textures.reserve(texCount);
    for (uint32_t i = 0; i < texCount; ++i) {
        uint32_t w = r.u32(), h = r.u32(), len = r.u32();
        uint32_t enc = r.u8();
        if (!r.need(len)) break;
        const void *bytes = data.data() + r.o;
        r.o += len;

        sf::Image img;
        Texture t;
        if (img.loadFromMemory(bytes, len)) {
            auto sz = img.getSize();
            t.w = int(sz.x);
            t.h = int(sz.y);
            t.px.resize(size_t(t.w) * t.h);
            const uint8_t *src = img.getPixelsPtr();
            for (size_t k = 0; k < t.px.size(); ++k)
                t.px[k] = packRGBA(src[k * 4], src[k * 4 + 1], src[k * 4 + 2], src[k * 4 + 3]);
        } else {
            std::fprintf(stderr, "assets: texture %u failed (%u bytes, enc %u)\n", i, len, enc);
            t = makeFallbackTexture();
        }
        (void)w; (void)h;
        textures.push_back(std::move(t));
    }

    materials.reserve(matCount);
    for (uint32_t i = 0; i < matCount; ++i) {
        AssetMaterial m;
        m.name = r.str();
        m.texture = r.i32();
        float rgba[4];
        for (float &v : rgba) v = r.f32();
        m.opacity = r.f32();
        int rr = int(clampf(rgba[0], 0, 1) * 255.0f + 0.5f);
        int gg = int(clampf(rgba[1], 0, 1) * 255.0f + 0.5f);
        int bb = int(clampf(rgba[2], 0, 1) * 255.0f + 0.5f);
        int aa = int(clampf(rgba[3], 0, 1) * 255.0f + 0.5f);
        m.color = packRGBA(rr, gg, bb, aa);
        if (m.opacity < 0.999f) m.blend = true;
        if (m.texture >= 0 && m.texture < int(textures.size()) && m.texture > -1) {
            const Texture &t = textures[size_t(m.texture)];
            for (uint32_t px : t.px) {
                if (((px >> 24) & 255) < 250) { m.blend = true; break; }
            }
        }
        materials.push_back(std::move(m));
    }

    meshes.reserve(meshCount);
    for (uint32_t i = 0; i < meshCount; ++i) {
        AssetMesh m;
        m.name = r.str();
        uint32_t vcount = r.u32();
        m.verts.resize(vcount);
        for (uint32_t k = 0; k < vcount; ++k) {
            m.verts[k].p.x = r.f32();
            m.verts[k].p.y = r.f32();
            m.verts[k].p.z = r.f32();
        }
        uint32_t ncount = r.u32();
        if (ncount == vcount) {
            for (uint32_t k = 0; k < vcount; ++k) {
                m.verts[k].n.x = r.f32();
                m.verts[k].n.y = r.f32();
                m.verts[k].n.z = r.f32();
            }
        } else {
            for (uint32_t k = 0; k < ncount * 3; ++k) r.f32();
            for (auto &v : m.verts) v.n = {0, 1, 0};
        }
        uint32_t ucount = r.u32();
        if (ucount == vcount) {
            for (uint32_t k = 0; k < vcount; ++k) {
                m.verts[k].u = r.f32();
                // The 3DOM UV origin is the bottom-left corner (OpenGL
                // convention) while the packed textures are stored top-down.
                // Without this flip every building sign and window band is
                // upside down.
                m.verts[k].v = 1.0f - r.f32();
            }
        } else {
            for (uint32_t k = 0; k < ucount * 2; ++k) r.f32();
        }
        uint32_t icount = r.u32();
        m.idx.resize(icount);
        for (uint32_t k = 0; k < icount; ++k) m.idx[k] = r.u32();
        uint32_t gcount = r.u32();
        m.groups.resize(gcount);
        for (uint32_t k = 0; k < gcount; ++k) {
            // The W3D parser reports groups in triangles; store index counts.
            m.groups[k].start = r.u32() * 3u;
            m.groups[k].count = r.u32() * 3u;
            m.groups[k].material = r.i32();
        }
        m.computeBounds();

        // Normalise triangle winding against the authored vertex normals so
        // back-face culling can be enabled safely. Some 3DOM resources are
        // authored clockwise; without this the whole tile disappears.
        if (!m.idx.empty()) {
            size_t front = 0, back = 0;
            for (size_t t = 0; t + 2 < m.idx.size(); t += 3) {
                const Vec3 &a = m.verts[m.idx[t]].p;
                const Vec3 &b = m.verts[m.idx[t + 1]].p;
                const Vec3 &c = m.verts[m.idx[t + 2]].p;
                Vec3 gn = (b - a).cross(c - a);
                Vec3 n = m.verts[m.idx[t]].n + m.verts[m.idx[t + 1]].n +
                         m.verts[m.idx[t + 2]].n;
                float d = gn.dot(n);
                if (d > 0) ++front; else if (d < 0) ++back;
            }
            if (back > front)
                for (size_t t = 0; t + 2 < m.idx.size(); t += 3)
                    std::swap(m.idx[t + 1], m.idx[t + 2]);
        }

        byName[m.name] = meshes.size();
        meshes.push_back(std::move(m));
    }

    if (!r.ok) {
        std::fprintf(stderr, "assets: truncated pack %s\n", path.c_str());
        return false;
    }
    std::printf("assets: %zu meshes, %zu materials, %zu textures (%.1f MB)\n",
                meshes.size(), materials.size(), textures.size(),
                double(data.size()) / (1024.0 * 1024.0));
    return true;
}

const AssetMesh *W3DAssets::mesh(const std::string &name) const {
    auto it = byName.find(name);
    return it == byName.end() ? nullptr : &meshes[it->second];
}

int W3DAssets::meshIndex(const std::string &name) const {
    auto it = byName.find(name);
    return it == byName.end() ? -1 : int(it->second);
}

const Texture *W3DAssets::texture(int material) const {
    if (material < 0 || material >= int(materials.size())) return nullptr;
    int t = materials[size_t(material)].texture;
    if (t < 0 || t >= int(textures.size())) return nullptr;
    return &textures[size_t(t)];
}

void drawAssetMesh(Renderer &r, const Camera &cam, const Mat4 &model,
                   const AssetMesh &mesh, bool unlit, float alphaScale, bool fog, int cull) {
    if (fog) r.setFog(packRGBA(176, 203, 226), 9000.0f, 42000.0f);
    // Opaque groups first, alpha-blended groups last so glass and decals keep
    // the depth buffer intact underneath them.
    for (int pass = 0; pass < 2; ++pass) {
        for (const AssetGroup &g : mesh.groups) {
            if (g.count == 0) continue;
            const AssetMaterial *m =
                (g.material >= 0 && g.material < int(gAssets.materials.size()))
                    ? &gAssets.materials[size_t(g.material)] : nullptr;
            bool blend = m ? m->blend : false;
            if ((pass == 0 && blend) || (pass == 1 && !blend)) continue;
            const Texture *tex = m ? gAssets.texture(g.material) : nullptr;
            uint32_t tint = m ? m->color : 0xffffffffu;
            float alpha = (m ? m->opacity : 1.0f) * alphaScale;
            r.drawMesh(cam, model, mesh, tex, tint, unlit, blend ? 0 : cull, alpha,
                       !blend, g.start, g.count);
        }
    }
}

} // namespace rr4
