#pragma once
// Loader for the original Shockwave 3D scene extracted from
// redlinerumble4.dcr (XMED-6650.bin). Geometry, UVs, normals, materials and
// JPEG/PNG textures are packed into assets/rr4_world.bin by the offline
// exporter; this keeps the game byte-for-byte on the original art.

#include "renderer.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace rr4 {

struct AssetGroup {
    uint32_t start = 0;
    uint32_t count = 0;
    int material = -1;
};

struct AssetMesh : Mesh {
    std::string name;
    std::vector<AssetGroup> groups;
    Vec3 min{1e30f, 1e30f, 1e30f}, max{-1e30f, -1e30f, -1e30f};
    void computeBounds();
};

struct AssetMaterial {
    std::string name;
    int texture = -1;
    uint32_t color = 0xffffffffu;
    float opacity = 1.0f;
    bool blend = false;
};

struct W3DAssets {
    std::vector<Texture> textures;
    std::vector<AssetMaterial> materials;
    std::vector<AssetMesh> meshes;
    std::unordered_map<std::string, size_t> byName;

    bool load(const std::string &path);
    const AssetMesh *mesh(const std::string &name) const;
    const Texture *texture(int material) const;
    int meshIndex(const std::string &name) const;
};

extern W3DAssets gAssets;

// Draws every material group of an original W3D mesh using the material's
// texture, diffuse colour and opacity.
void drawAssetMesh(Renderer &r, const Camera &cam, const Mat4 &model,
                   const AssetMesh &mesh, bool unlit = false,
                   float alphaScale = 1.0f, bool fog = true, int cull = 1);

} // namespace rr4
