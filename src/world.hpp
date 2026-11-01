#pragma once
#include "assets.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include <array>
#include <cmath>
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
    bool ready = false;

    void build();
    void draw(Renderer &r, const Camera &cam);
    // doCollisions(): the tile radial pushes. Safe to call from inside
    // Car::update, before the car moves (frame order matters, see Car::update).
    void resolveCarCollision(Car &car) const;
    // doRayCasting(): ground height and surface tilt, plus the map edge fence.
    // Call after the step.
    void updateGround(Car &car) const;

    // Highest original tile surface under (x, z) that is reachable from nearY.
    float groundHeightAt(float x, float z, float nearY) const;

    // Original: onTileCol = integer(worldPosition.x / 5000 + 0.5). Lingo's
    // integer() rounds, so the tile owning x is floor(x / 5000) + 1 and its
    // centre (mVisTileList worldPosition) is col * 5000 - 2500.
    int tileColAt(float x) const { return int(std::floor(x / TILE_SIZE)) + 1; }
    int tileRowAt(float z) const { return int(std::floor(z / TILE_SIZE)) + 1; }
    int tileIndexAt(float x, float z) const {
        int c = tileColAt(x), rw = tileRowAt(z);
        if (c < 1 || c > GRID || rw < 1 || rw > GRID) return -1;
        return (c - 1) + (rw - 1) * GRID;
    }
    int tileIDAt(float x, float z) const {
        int i = tileIndexAt(x, z);
        return i < 0 ? 0 : kTileID[i];
    }
    static Vec3 tileCenter(int col, int row) {
        return {col * TILE_SIZE - TILE_SIZE * 0.5f, 0.0f,
                row * TILE_SIZE - TILE_SIZE * 0.5f};
    }

private:
    void buildProtos();
    void buildHeights();
    void buildProps();
};

} // namespace rr4
