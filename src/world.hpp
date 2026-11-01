#pragma once
#include "assets.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include <array>
#include <vector>
#include <string>

namespace rr4 {

struct Car;

constexpr float TILE_SIZE = 5000.0f;
constexpr int GRID = 12;

// Verbatim from BehaviorScript 4 - PlayerCar.ls (tileIDList / tileRotList).
extern const int kTileID[GRID * GRID];
extern const int kTileRot[GRID * GRID];

const char *tileName(int id);
bool isRoadTile(int id);

struct AABB {
    Vec3 min, max;
};

// One material batch of an original tile resource (base mesh, objects mesh or
// baked shadow plane from XMED-6650.bin).
struct TileBatch {
    Mesh mesh;
    int material = -1;
    bool blend = false;
};

struct TileProto {
    std::vector<TileBatch> batches;
    AABB bounds{};
};

struct WorldProp {
    int mesh = -1;
    Mat4 model = Mat4::identity();
    bool unlit = false;
    float alpha = 1.0f;
};

// One 128x128 column of original tile surface heights. Loops and bridges can
// stack several decks above a cell, so up to four ys are kept.
static constexpr int HMAP = 128;
struct HeightCell {
    float y[4] = {-1e30f, -1e30f, -1e30f, -1e30f};
};

// The 12 x 12 map assembled from the original Shockwave 3D tile resources.
struct World {
    std::array<TileProto, 38> protos;
    std::array<std::vector<HeightCell>, 38> heights;
    std::vector<WorldProp> props;
    std::vector<AABB> colliders;
    bool ready = false;

    void build();
    void draw(Renderer &r, const Camera &cam);
    void resolveCarCollision(Car &car) const;

    // Highest original tile surface under (x, z) that is reachable from nearY.
    float groundHeightAt(float x, float z, float nearY) const;

    int tileColAt(float x) const { return int(x / TILE_SIZE + 0.5f); }
    int tileRowAt(float z) const { return int(z / TILE_SIZE + 0.5f); }
    int tileIndexAt(float x, float z) const {
        int c = tileColAt(x), rw = tileRowAt(z);
        if (c < 0 || c >= GRID || rw < 1 || rw > GRID) return -1;
        return c + (rw - 1) * GRID;
    }
    int tileIDAt(float x, float z) const {
        int i = tileIndexAt(x, z);
        return i < 0 ? 0 : kTileID[i];
    }
    static Vec3 tileCenter(int col, int row) {
        return {col * TILE_SIZE, 0.0f, row * TILE_SIZE};
    }

private:
    void buildProtos();
    void buildHeights();
    void buildProps();
    void buildColliders();
};

} // namespace rr4
