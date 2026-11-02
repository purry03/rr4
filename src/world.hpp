#pragma once
#include "colortank.hpp"
#include "ferris.hpp"
#include "garage.hpp"
#include "plane.hpp"
#include "race.hpp"
#include "stage.hpp"
#include "traffic.hpp"
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
// baked shadow plane from XMED-6650.bin). The batch points into the loaded
// asset mesh instead of copying its vertices: a tile uses the same mesh in
// several material groups and the copy cost was ~20 MB of RAM.
struct TileRange {
    const AssetMesh *mesh = nullptr;
    size_t indexBegin = 0, indexCount = 0;
};

struct TileBatch {
    std::vector<TileRange> ranges;
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
    int rixDoorProp = -1;           // RixGarageDoor_000 in props
    FerrisWheel ferris;
    Race race;
    Stage stage;
    ColorTank colorTank;
    // g_CarsColorIDList: paint job 1..kPaintJobs per car.
    int carColors[kCarCount] = {kDefaultCarColors[0], kDefaultCarColors[1], kDefaultCarColors[2],
                                kDefaultCarColors[3], kDefaultCarColors[4], kDefaultCarColors[5]};
    Traffic traffic;
    Plane plane;
    float radarAngle = 0.0f;        // mRadar spin about Y, degrees
    // Tiles drawn in each direction around the camera (original: 2).
    static constexpr int kMinDrawDistance = 2, kMaxDrawDistance = 5;
    static constexpr float kFogMargin = 500.0f, kFogStartShare = 0.25f;
    int drawDistance = 3;
    long ticks = 0;                 // the ticks, one per step
    bool objectsReady = false;
    bool ambientReady = false;      // traffic, plane and wheel set up
    // g_CarsOwnedList: 2 = driven, 1 = waiting in the Ferris wheel, 0 = not
    // owned. Starts as the original save: only the driven car.
    int carsOwned[kCarCount] = {};   // g_CarsOwnedList (Ownership)
    struct NitroPickup { Vec3 pos; bool visible; };
    std::vector<NitroPickup> nitroPickups;   // mIgNitroList
    int nitroMesh = -1;
    int radarMesh = -1;             // AirPortRadar_000
    bool firstIgNitro = true;
    bool ready = false;

    void build();
    void draw(Renderer &r, const Camera &cam);
    // doCollisions(): the tile radial pushes. Safe to call from inside
    // Car::update, before the car moves (frame order matters, see Car::update).
    void resolveCarCollision(Car &car) const;
    // One step's doTraffic and doTrafficLights, then the whole of
    // doCollisions (traffic pushes, then the tiles). Use this in the step;
    // resolveCarCollision alone is for placing a car.
    void doCollisions(Car &car);
    // mRixGarageDoor.worldPosition
    static Vec3 rixDoorPos();
    // mRadar.transform.xAxis
    Vec3 radarXAxis() const {
        float a = deg2rad(radarAngle);
        return {std::cos(a), 0.0f, -std::sin(a)};
    }
    // The ported enterFrame handlers, run between doCollisions and
    // doBodyRoll: doOpponent, doFerrisWheel, doRixRapidRepairs, doIgNitro,
    // doRaceFlag and doRacePods.
    void doWorldObjects(Car &car, const CarInput &in);
    // The world without a player (main menu): traffic and lights around
    // `focus`, the plane, the radar dish and the Ferris wheel.
    void updateAmbient(const Vec3 &focus);
    // Start over (New Game / first start): gameplay systems back to their
    // setup state with this ownership and paint.
    void newSession(const int owned[kCarCount], const int colors[kCarCount]);
    static constexpr float kRadarDegPerFrame = 5.0f;   // doAnims mRadar.rotate
    // doRayCasting(): twelve cage rays, the hoist and the suspension torques.
    // Runs first in the step, before doControls.
    void updateGround(Car &car) const;

    // modelsUnderRay(): the first tile surface under (x, z) hit by a downward
    // ray starting kGroundRayAbove over nearY. False when the ray hits nothing
    // (e.g. a cage point over the wall of the airport tunnel); the original
    // then adds no imbed for that point.
    bool groundRayAt(float x, float z, float nearY, float &y) const;
    // groundRayAt blended between the four nearest cells that it hits.
    bool groundRaySmooth(float x, float z, float nearY, float &y) const;
    // Highest original tile surface under (x, z) that is reachable from nearY,
    // or the lowest surface there when nothing is (objects that must always
    // stand on something: traffic, camera).
    float groundHeightAt(float x, float z, float nearY) const;
    // groundHeightAt blended between the four nearest cells, for objects
    // that follow the surface every frame (traffic) and must not step.
    float groundHeightSmooth(float x, float z, float nearY) const;
    // Surface normal of the tile height field at (x, z), for doRayCasting.
    Vec3 groundNormalAt(float x, float z, float nearY) const;
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
    // Empty HeightCell slot marker.
    static constexpr float kNoSurface = -1e29f;
    // doRayCasting starts the ray 60 above the cage point and callers pass
    // nearY = point - 80, so the ray top is nearY + 140.
    static constexpr float kGroundRayAbove = 140.0f;
    const HeightCell *heightCellAt(float x, float z) const;
    void buildProtos();
    void buildHeights();
    void buildProps();
    void setupAmbient();
    void doRixRapidRepairs(Car &car);
    void doIgNitro(Car &car);
};

} // namespace rr4
