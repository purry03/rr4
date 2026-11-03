#include "world.hpp"
#include "car.hpp"
#include "collision.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

namespace rr4 {

const int kTileID[GRID * GRID] = {
    27, 19, 5, 9, 9, 9, 9, 11, 11, 11, 11, 11,
    19, 13, 17, 3, 4, 3, 11, 11, 2, 11, 2, 12,
    16, 14, 17, 11, 10, 11, 11, 2, 11, 2, 2, 8,
    19, 14, 17, 6, 7, 6, 6, 7, 7, 7, 6, 8,
    18, 15, 28, 6, 7, 6, 6, 7, 7, 7, 6, 8,
    19, 15, 28, 35, 36, 35, 35, 36, 35, 37, 35, 8,
    18, 15, 28, 2, 2, 2, 20, 2, 2, 2, 2, 8,
    18, 15, 28, 2, 2, 2, 2, 2, 2, 2, 2, 12,
    19, 14, 34, 23, 23, 23, 21, 23, 23, 22, 2, 2,
    19, 14, 29, 25, 23, 24, 23, 23, 23, 25, 2, 29,
    19, 13, 33, 22, 25, 23, 22, 25, 26, 23, 2, 30,
    27, 27, 31, 32, 32, 32, 32, 32, 32, 32, 32, 31,
};

const int kTileRot[GRID * GRID] = {
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 2, 1, 2, 2, 2, 2, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 0, 3, 0, 0, 3, 0, 0, 0,
    0, 0, 2, 0, 2, 0, 0, 2, 1, 0, 0, 2,
    0, 0, 0, 2, 1, 1, 2, 3, 1, 1, 0, 2,
    3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2,
};

static const char *kTileNames[38] = {
    "", "DistantHill", "CrossRoads", "FinancialFlyOver", "AshTrayRoundabout",
    "SplitLevelCarPark", "CiscoHillCorner", "CiscoHillSide", "BridgeSection",
    "FinancialBase", "TahTeeCosmetics", "FinancialCrossRoads", "BridgeOffRamps",
    "RunwayA", "RunwayB", "RunwayC", "ControlTower", "AirportBuilding", "Hangar",
    "FuelDump", "FerrisWheel", "NurdBurgerRing", "NurdBurgerLoop", "NurdBurgerCorner",
    "NurdBurgerDungLoopBridge", "NurdBurgerStraight", "NurdBurgerHill",
    "AirportCorner", "Fence", "TunnelEntrance", "TunnelWaveOut", "TunnelCurve",
    "Tunnel", "TunnelWaveIn", "TunnelFence", "DockCanal", "DockCanalBridge",
    "DockCanalRaisedBridge",
};

const char *tileName(int id) {
    return (id >= 0 && id <= 37) ? kTileNames[id] : "?";
}

bool isRoadTile(int id) {
    switch (id) {
    case 2: case 3: case 4: case 6: case 7: case 8: case 9: case 11:
    case 12: case 13: case 14: case 15: case 21: case 22: case 23: case 24:
    case 25: case 26: case 27: case 29: case 30: case 31: case 32: case 33:
    case 34: case 35: case 36: case 37:
        return true;
    default:
        return false;
    }
}

namespace {

// gCageList from doSetupPlayerCar(): twelve ray origins on the car body. The
// first four are the wheel contact points (x = +-50, z = +-60, y = 30).
// Points 1/2 are rear, 3/4 front; odd = left (local +x), even = right.
// doSetupigNitro / doResetFerrisWheel: the free-drive bottle.
const Vec3 kFerrisNitroPos{34017.54f, 50.0f, 32709.34f};

// doSetupRixRapidRepairs: RixGarageDoor_000 world position.
const Vec3 kRixDoorPos{32500.0f + 1152.0f, 440.0f - 378.0f, 32500.0f + 1050.0f};

const Vec3 kCage[12] = {
    {-50.0f, 30.0f, -60.0f}, {50.0f, 30.0f, -60.0f},
    {-50.0f, 30.0f, 60.0f},  {50.0f, 30.0f, 60.0f},
    {-55.0f, 40.0f, -60.0f}, {55.0f, 40.0f, -60.0f},
    {-55.0f, 45.0f, 60.0f},  {55.0f, 45.0f, 60.0f},
    {-50.0f, 90.0f, -10.0f}, {50.0f, 90.0f, -10.0f},
    {-50.0f, 100.0f, 45.0f}, {50.0f, 100.0f, 45.0f},
};

// Exact resource names from the 3D cast member, in mTileResourceList order.
const char *kBaseName[38] = {
    nullptr,
    "TILE_DistantHill_002", "TILE_CrossRoads_003", "TILE_FinancialFlyOver_000",
    "TILE_AshTrayRoundabout_001", "TILE_SplitLevelCarPark_008",
    "TILE_CiscoHillCorner_001", "TILE_CiscoHillSide_001", "TILE_BridgeSection_001",
    "TILE_FinancialBase_000", "TILE_TahTeeCosmetics_000",
    "TILE_FinancialCrossRoads_001", "TILE_BridgeOffRamps_001",
    "TILE_AirPortRunway_A_000", "TILE_AirPortRunway_B_000",
    "TILE_AirPortRunway_C_000", "TILE_AirPortControlTower_000",
    "TILE_AirPortBuilding_001", "TILE_AirPortHangar_002",
    "TILE_AirPortFuelDump_000", "TILE_FerrisWheel_000",
    "TILE_NurdBurgerRing_003", "TILE_NurdBurgerRing_Loop_002",
    "TILE_NurdBurgerRing_Corner_001", "TILE_NurdBurgerRing_DungLoopBridge_000",
    "TILE_NurdBurgerRing_Straight_000", "TILE_NurdBurgerRing_Hill_000",
    "TILE_AirPortCorner_000", "TILE_AirPortFence_000",
    "TILE_AirPortTunnelEntrance_000", "TILE_AirPortTunnelWaveOut_000",
    "TILE_AirPortTunnelCurve_000", "TILE_AirPortTunnel_000",
    "TILE_AirPortTunnelWaveIn_000", "TILE_AirPortTunnelFence_000",
    "TILE_DockCanal_001", "TILE_DockCanalBridge_000",
    "TILE_DockCanalRaisedBridge_000",
};

const char *kObjectsName[38] = {
    nullptr,
    "TILE_DistantHillObjects_000 (1)", "TILE_CrossRoadsObjects_000",
    "TILE_FinancialFlyOverObjects_000", "TILE_AshTrayRoundaboutObjects_000",
    "TILE_SplitLevelCarParkObjects_000", "TILE_CiscoHillCornerObjects_000",
    "TILE_CiscoHillSideObjects_001", nullptr,
    "TILE_FinancialBaseObjects_000", nullptr,
    "TILE_FinancialCrossRoadsObjects_000", nullptr,
    nullptr, nullptr, nullptr, "TILE_AirPortControlTower_Objects_000",
    "TILE_AirPortBuildingObjects_000", nullptr, nullptr,
    "TILE_FerrisWheelObjects_002", "TILE_NurdBurgerRingObjects_001",
    "TILE_NurdBurgerRing_LoopObjects_001", "TILE_NurdBurgerRing_CornerObjects_000",
    "TILE_NurdBurgerRing_DungLoopBridgeObjects_000",
    "TILE_NurdBurgerRing_StraightObjects_000", "TILE_NurdBurgerRing_HillObjects_000",
    "TILE_AirPortCornerObjects_000", "TILE_AirPortFenceObjects_000",
    "TILE_AirPortTunnelEntranceObjects_001", "TILE_AirPortTunnelWaveOutObjects_000",
    "TILE_AirPortTunnelCurveObjects_000", "TILE_AirPortTunnelObjects_000",
    "TILE_AirPortTunnelWaveInObjects_000", "TILE_AirPortTunnelFenceObjects_000",
    "TILE_DockCanalObjects_001", "TILE_DockCanalBridgeObjects_000",
    "TILE_DockCanalRaisedBridgeObjects_000",
};

// Baked shadow maps shipped with each tile in the original 3DEM stream.
const char *kShadowName[38] = {
    nullptr,
    "TILE_DistantHillShadow_000", "TILE_CrossRoadsShadow_000 (1)",
    "TILE_FinancialShadow_000", "TILE_AshTrayRoundaboutShadow_000",
    "TILE_SplitLevelCarParkShadow_001", "TILE_CiscoHillCornerShadow_000",
    "TILE_CiscoHillSideShadow_001", "TILE_BridgeSectionShadow_000",
    "TILE_FinancialBaseShadow_000", "TILE_TahTeeCosmeticsShadow_000",
    "TILE_CrossRoadsShadow_000 (2)", "TILE_BridgeOffRampsShadow_000",
    "TILE_AirPortRunway_A_Shadow_000 (1)", "TILE_AirPortRunway_A_Shadow_000 (2)",
    "TILE_AirPortRunway_A_Shadow_000 (3)", "TILE_AirPortControlTower_Shadow_000",
    "TILE_AirPortBuildingShadow_000 (1)", "TILE_AirPortHangar_Shadow_001",
    "TILE_AirPortFuelDumpShadow_000", "TILE_FerrisWheelShadow_001",
    "TILE_NerdBurgerRingShadow_000", "TILE_NurdBurgerRing_LoopShadow_001",
    "TILE_NurdBurgerRing_CornerShadow_001",
    "TILE_NurdBurgerRing_DungLoopBridgeShadow_000",
    "TILE_NurdBurgerRing_StraightShadow_000", "TILE_NurdBurgerRing_HillShadow_000",
    "TILE_AirPortRunway_A_Shadow_000 (4)", "TILE_AirPortBuildingShadow_000 (2)",
    "TILE_AirPortTunnelEntranceShadow_000", "TILE_AirPortTunnelShadow_000 (1)",
    "TILE_AirPortTunnelShadow_000 (2)", "TILE_AirPortTunnelShadow_000 (3)",
    "TILE_AirPortTunnelShadow_000 (4)", "TILE_AirPortTunnelFenceShadow_000",
    "TILE_DockCanalShadow_000", "TILE_DockCanalBridgeShadow_000",
    "TILE_DockCanalBridgeRaisedShadow_000",
};

void appendSource(std::vector<TileBatch> &batches,
                  const std::vector<int> &matToBatch,
                  const AssetMesh &src) {
    for (const AssetGroup &g : src.groups) {
        if (g.count == 0 || g.material < 0 ||
            g.material >= int(matToBatch.size()))
            continue;
        int bi = matToBatch[size_t(g.material)];
        if (bi < 0) continue;
        TileBatch &b = batches[size_t(bi)];
        b.ranges.push_back({&src, g.start, g.count});
    }
}

bool assetFile(std::string &out) {
    const char *candidates[] = {
        "assets/rr4_world.bin", "../assets/rr4_world.bin",
        "../../assets/rr4_world.bin", "/home/perry/Dev/rr4/assets/rr4_world.bin",
    };
    for (const char *c : candidates) {
        if (std::filesystem::exists(c)) { out = c; return true; }
    }
    return false;
}

} // namespace

void World::buildProtos() {
    for (int id = 1; id <= 37; ++id) {
        TileProto &proto = protos[size_t(id)];
        proto.batches.clear();

        // Materials already used by this tile -> batch index.
        std::vector<int> matToBatch(gAssets.materials.size(), -1);

        auto addMesh = [&](const char *name) {
            if (!name) return;
            const AssetMesh *m = gAssets.mesh(name);
            if (!m) {
                std::fprintf(stderr, "world: missing mesh '%s'\n", name);
                return;
            }
            for (const AssetGroup &g : m->groups) {
                if (g.material < 0 || g.material >= int(matToBatch.size())) continue;
                if (matToBatch[size_t(g.material)] < 0) {
                    matToBatch[size_t(g.material)] = int(proto.batches.size());
                    TileBatch b;
                    b.material = g.material;
                    b.blend = gAssets.materials[size_t(g.material)].blend;
                    proto.batches.push_back(std::move(b));
                }
            }
            appendSource(proto.batches, matToBatch, *m);
        };

        addMesh(kBaseName[id]);
        addMesh(kObjectsName[id]);
        // The original disables the baked shadow pass when it runs on a
        // software renderer (shadowsOn = 0), so the 3DEM shadow planes are
        // skipped here as well.
        (void)kShadowName;

        // Tile bounds from the base mesh (always a 5000-unit tile).
        const AssetMesh *base = gAssets.mesh(kBaseName[id]);
        proto.bounds.min = base ? base->min : Vec3{-2500, -500, -2500};
        proto.bounds.max = base ? base->max : Vec3{2500, 500, 2500};
    }
}

void World::buildHeights() {
    for (auto &h : heights) h.clear();
    for (int id = 1; id <= 37; ++id) {
        auto &grid = heights[size_t(id)];
        grid.assign(size_t(HMAP) * HMAP, HeightCell{});
        auto insert = [](HeightCell &c, float y) {
            for (int i = 0; i < 4; ++i)
                if (std::fabs(c.y[i] - y) < 1.0f) return;
            if (y <= c.y[3]) return;
            int p = 3;
            while (p > 0 && c.y[p - 1] < y) { c.y[p] = c.y[p - 1]; --p; }
            c.y[p] = y;
        };
        auto raster = [&](const AssetMesh *mesh) {
            if (!mesh) return;
            for (size_t t = 0; t + 2 < mesh->idx.size(); t += 3) {
                const Vec3 &a = mesh->verts[mesh->idx[t]].p;
                const Vec3 &b = mesh->verts[mesh->idx[t + 1]].p;
                const Vec3 &c = mesh->verts[mesh->idx[t + 2]].p;
                float den = (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z);
                if (std::fabs(den) < 1e-6f) continue;
                float minX = std::min({a.x, b.x, c.x}), maxX = std::max({a.x, b.x, c.x});
                float minZ = std::min({a.z, b.z, c.z}), maxZ = std::max({a.z, b.z, c.z});
                int gx0 = std::max(0, int((minX + 2500.0f) / TILE_SIZE * HMAP));
                int gx1 = std::min(HMAP - 1, int((maxX + 2500.0f) / TILE_SIZE * HMAP));
                int gz0 = std::max(0, int((minZ + 2500.0f) / TILE_SIZE * HMAP));
                int gz1 = std::min(HMAP - 1, int((maxZ + 2500.0f) / TILE_SIZE * HMAP));
                for (int gz = gz0; gz <= gz1; ++gz) {
                    float pz = -2500.0f + (float(gz) + 0.5f) / HMAP * TILE_SIZE;
                    for (int gx = gx0; gx <= gx1; ++gx) {
                        float px = -2500.0f + (float(gx) + 0.5f) / HMAP * TILE_SIZE;
                        float u = ((px - a.x) * (c.z - a.z) - (c.x - a.x) * (pz - a.z)) / den;
                        float v = ((b.x - a.x) * (pz - a.z) - (px - a.x) * (b.z - a.z)) / den;
                        if (u < -0.001f || v < -0.001f || u + v > 1.001f) continue;
                        float y = a.y + u * (b.y - a.y) + v * (c.y - a.y);
                        insert(grid[size_t(gz) * HMAP + gx], y);
                    }
                }
            }
        };
        // The base resource is the ground; the objects resource adds the
        // driveable road where it is not part of the base (the airport tunnel
        // interior, the canal bridges). Roofs and other high surfaces are
        // ignored by groundHeightAt's pick (it takes the highest surface at or
        // below the car), so they only ever add a deck the car could land on.
        // The base resource is the ground; for the airport tunnel tiles the
        // driveable road is part of the objects resource instead (the base is
        // just a flat floor slab), so those also rasterize their objects mesh.
        // The original doRayCasting casts against the tile's base resource
        // only, which is why these tiles were empty inside the tunnel.
        if (kBaseName[id]) raster(gAssets.mesh(kBaseName[id]));
        if (id >= 29 && id <= 33 && kObjectsName[id]) raster(gAssets.mesh(kObjectsName[id]));
    }
}

const HeightCell *World::heightCellAt(float x, float z) const {
    int idx = tileIndexAt(x, z);
    if (idx < 0) return nullptr;
    int id = kTileID[idx];
    if (id <= 0 || id > 37 || heights[size_t(id)].empty()) return nullptr;

    Vec3 center = tileCenter(idx % GRID + 1, idx / GRID + 1);
    Vec3 d{x - center.x, 0.0f, z - center.z};
    Mat4 inv = Mat4::rotationY(deg2rad(90.0f * float(kTileRot[idx])));
    Vec3 local = inv.transformVector(d);

    int gx = int((local.x + 2500.0f) / TILE_SIZE * HMAP);
    int gz = int((local.z + 2500.0f) / TILE_SIZE * HMAP);
    gx = std::max(0, std::min(HMAP - 1, gx));
    gz = std::max(0, std::min(HMAP - 1, gz));
    return &heights[size_t(id)][size_t(gz) * HMAP + gx];
}

bool World::groundRayAt(float x, float z, float nearY, float &y) const {
    const HeightCell *c = heightCellAt(x, z);
    // Off the map or a tile without a height field: flat ground at 0.
    if (!c) { y = 0.0f; return true; }
    float best = kNoSurface;
    bool any = false;
    for (float s : c->y) {
        if (s <= kNoSurface) continue;
        any = true;
        if (s <= nearY + kGroundRayAbove && s > best) best = s;
    }
    // A cell no triangle covers is a gap in the rasterized height field, not
    // a hole in the original mesh: keep it flat at 0 as before.
    if (!any) { y = 0.0f; return true; }
    if (best <= kNoSurface) return false;
    y = best;
    return true;
}

float World::groundHeightAt(float x, float z, float nearY) const {
    float y;
    if (groundRayAt(x, z, nearY, y)) return y;
    // Nothing at or below nearY: snap up to the lowest surface instead.
    const HeightCell *c = heightCellAt(x, z);
    float lowest = -kNoSurface;
    for (float s : c->y)
        if (s > kNoSurface) lowest = std::min(lowest, s);
    return lowest < -kNoSurface ? lowest : 0.0f;
}

bool World::groundRaySmooth(float x, float z, float nearY, float &y) const {
    // Same lattice as groundHeightSmooth, but cells the ray misses (beside the
    // airport tunnel floor, nothing lies below a car inside it) drop out of
    // the blend instead of pulling the surface up to the roof slab.
    const float cell = TILE_SIZE / float(HMAP);
    float fx = x / cell - 0.5f, fz = z / cell - 0.5f;
    float x0 = std::floor(fx), z0 = std::floor(fz);
    float tx = fx - x0, tz = fz - z0;
    float sum = 0.0f, weight = 0.0f;
    for (int k = 0; k < 2; ++k)
        for (int i = 0; i < 2; ++i) {
            float w = (i ? tx : 1.0f - tx) * (k ? tz : 1.0f - tz);
            float h;
            if (w <= 0.0f ||
                !groundRayAt((x0 + float(i) + 0.5f) * cell, (z0 + float(k) + 0.5f) * cell, nearY, h))
                continue;
            sum += h * w;
            weight += w;
        }
    if (weight <= 0.0f) return false;
    y = sum / weight;
    return true;
}

float World::groundHeightSmooth(float x, float z, float nearY) const {
    // Height cells sit on a world-aligned lattice (tile corners are multiples
    // of 5000 and tiles turn in 90 degree steps), centres at (i + 0.5) * cell.
    const float cell = TILE_SIZE / float(HMAP);
    float fx = x / cell - 0.5f, fz = z / cell - 0.5f;
    float x0 = std::floor(fx), z0 = std::floor(fz);
    float tx = fx - x0, tz = fz - z0;
    auto h = [&](float i, float k) {
        return groundHeightAt((i + 0.5f) * cell, (k + 0.5f) * cell, nearY);
    };
    float a = h(x0, z0) + (h(x0 + 1, z0) - h(x0, z0)) * tx;
    float b = h(x0, z0 + 1) + (h(x0 + 1, z0 + 1) - h(x0, z0 + 1)) * tx;
    return a + (b - a) * tz;
}

void World::buildProps() {
    auto add = [&](const char *name, const Mat4 &xf, bool unlit = false,
                   float alpha = 1.0f) {
        int i = gAssets.meshIndex(name);
        if (i < 0) std::fprintf(stderr, "world: missing prop '%s'\n", name);
        else props.push_back({i, xf, unlit, alpha});
    };

    // doSetupTallObjects
    add("TALLOBJECT_SunnyPierceToole_000", Mat4::translation({17500, 115, 1000}));
    add("TALLOBJECT_GoldenGateBridge_001", Mat4::translation({57500, 0, 27500}));
    add("TALLOBJECT_GoldenGateBridge_001",
        Mat4::translation({57500, 0, 27500}) * Mat4::rotationY(PI) *
            Mat4::translation({0, 0, 10000}));
    add("TALLOBJECT_WellardScrewDriver_000", Mat4::translation({27500, 115, 800}));
    add("TALLOBJECT_TahTeeCosmetics_000",
        Mat4::translation({22500, 0, 12500}) * Mat4::rotationY(deg2rad(90)));
    add("TALLOBJECT_Lunkopf&StubbsCigarette_000",
        Mat4::translation({22500, 115, 800}));
    add("TALLOBJECT_OuterWalls_000", Mat4::identity());

    // doSetupRixRapidRepairs: the shutter sits at mFerrisWheel.worldPosition
    // (32500, 440, 32500) + (1152, -378, 1050), rotation (0, 0, 0).
    size_t before = props.size();
    add("RixGarageDoor_000", Mat4::translation(kRixDoorPos));
    if (props.size() > before) rixDoorProp = int(before);

    // doSetupigNitro: ten bottle clones; in free drive only the first is
    // shown, at (34017.54, 50, 32709.34). The race/stage lists place the
    // others when those modes start, which this build does not have.
    radarMesh = gAssets.meshIndex("AirPortRadar_000");
    nitroMesh = gAssets.meshIndex("igNitroBottle_001");
    // Ten clones; races reuse the first ones for their own bottles.
    nitroPickups.assign(10, {{}, false});
    nitroPickups[0] = {kFerrisNitroPos, true};
}

void World::setupAmbient() {
    if (ambientReady) return;
    traffic.setup();
    plane.setup();
    ferris.setup(carsOwned);
    ambientReady = true;
}

void World::updateAmbient(const Vec3 &focus) {
    if (!ready) return;
    setupAmbient();
    ++ticks;
    traffic.update(*this, focus, ticks);
    traffic.updateLights(ticks);
    plane.update(ticks);
    ferris.cullTo(focus);
    radarAngle = std::fmod(radarAngle + kRadarDegPerFrame, 360.0f);
}

void World::newSession(const int owned[kCarCount], const int colors[kCarCount]) {
    ferris = FerrisWheel{};
    race = Race{};
    stage = Stage{};
    colorTank = ColorTank{};
    objectsReady = false;
    ambientReady = false;
    firstIgNitro = true;
    nitroPickups.assign(nitroPickups.size(), {{}, false});
    if (!nitroPickups.empty()) nitroPickups[0] = {kFerrisNitroPos, true};
    for (int i = 0; i < kCarCount; ++i) {
        carsOwned[i] = owned[i];
        carColors[i] = colors[i];
        setCarPaint(i + 1, colors[i]);
    }
}

void World::doWorldObjects(Car &car, const CarInput &in) {
    if (!ready) return;   // tile-lookup-only worlds (physics/camera traces)
    if (!objectsReady) {
        setupAmbient();
        carsOwned[size_t(car.carId - 1)] = kOwnedDriving;
        ferris.setup(carsOwned);
        race.setup();
        stage.setup();
        colorTank.setup();
        objectsReady = true;
    }
    race.updateOpponent(car, *this);
    // doResetFerrisWheel also puts the first igNitro bottle back.
    if (ferris.update(car, in, 1.0f)) {
        nitroPickups[0].pos = kFerrisNitroPos;
        nitroPickups[0].visible = true;
    }
    // doFerrisWheel: coming back to the wheel mid-race abandons it.
    if (ferris.inRange() && ferris.state() == 0 && race.state() == 2) race.abandon(car, *this);
    plane.update(ticks);   // doPlane, after doCollisions
    doRixRapidRepairs(car);
    colorTank.update(car, in, carColors);   // doColorTank
    doIgNitro(car);
    race.updateFlag(car, *this);
    stage.update(car, *this);   // doPointsFlag, doPointPods
    // doAnims: mRadar.rotate(0, 5 * g_FPSFactor, 0)
    radarAngle = std::fmod(radarAngle + kRadarDegPerFrame, 360.0f);
}

// doIgNitro: drive within 150 of a visible bottle to collect it. The first
// pickup shows panel 16; every pickup plays Sfx 25, refills g_BoostPercent
// and flips the boot bottle (igNitroRot = 250, decaying 10 % per frame).
void World::doIgNitro(Car &car) {
    for (NitroPickup &p : nitroPickups) {
        if (!p.visible || (car.pos - p.pos).length() >= 150.0f) continue;
        p.visible = false;
        if (firstIgNitro) {
            firstIgNitro = false;
            car.doPanel(16, 300);
        }
        car.nitroPickedUp = true;
        car.igNitroRot = 250.0f;
        car.boostMeter = 1.0f;
    }
    car.igNitroRot *= 0.9f;
    if (car.igNitroRot < 0.1f) car.igNitroRot = 0.0f;
}

void World::doRixRapidRepairs(Car &car) {
    Vec3 diff = kRixDoorPos - car.pos;
    diff.y = 0.0f;
    // The shutter opens as the car approaches: fully open 165 units in front.
    float diffZ = 65.0f - (diff.z - 100.0f);
    float open = clampf(1.0f - std::fabs(diffZ) / 380.0f, 0.0f, 1.0f);
    if (rixDoorProp >= 0)
        props[size_t(rixDoorProp)].model =
            Mat4::translation(kRixDoorPos) * Mat4::rotationX(deg2rad(90.0f * open));

    const bool inside = diff.z > -420.0f && diff.z < 60.0f;
    auto straighten = [&] {
        car.pos.x = kRixDoorPos.x;
        // mPlayerCar.transform.rotation * 0.95
        car.setRotationDeg(car.rotationDeg() * 0.95f);
    };
    if (car.repairState == 0) {
        if (std::fabs(diff.x) < 200.0f && inside) {
            car.repairState = 1;
            car.doPanel(9, 0);
            straighten();
        }
    } else if (std::fabs(diff.x) < 200.0f) {
        if (inside) {
            straighten();
            if (diff.z < -210.0f) {
                car.repairSfx = true;
                car.pos.z = kRixDoorPos.z + 210.0f;
                car.vel.z = 0.0f;
                car.damage = 0.0f;
                car.resetParts();
            }
        }
    } else {
        car.repairState = 0;
        car.doPanel(0, 0);
        car.saveGame = true;
    }
}

void World::build() {
    std::string path;
    if (!assetFile(path) || !gAssets.load(path)) {
        std::fprintf(stderr, "world: original assets unavailable; run from the repo root\n");
        ready = false;
        return;
    }
    buildProtos();
    buildHeights();
    buildProps();
    ready = true;
    std::printf("world: %d tile resources, %zu props\n",
                int(protos.size()) - 1, props.size());
}

void World::draw(Renderer &r, const Camera &cam) {
    if (!ready) return;
    r.setLights(kSceneLights, kSceneLightCount);

    int pc = tileColAt(cam.pos.x);
    int pr = tileRowAt(cam.pos.z);
    // Tiles drawn around the camera (the original visTileDist is 2). The fog
    // is solid kFogMargin before the nearest undrawn tile and starts at
    // kFogStartShare of that.
    const int dist = std::clamp(drawDistance, kMinDrawDistance, kMaxDrawDistance);
    const float fogEnd = float(dist) * TILE_SIZE - kFogMargin;
    r.setWorldFogRange(fogEnd * kFogStartShare, fogEnd);
    r.setWorldFog();

    // Only reject a tile when its whole square (half-diagonal radius) is
    // behind the camera plane. Anything that can reach the image is still
    // drawn, so road next to and behind the car never disappears.
    Vec3 fwd = cam.target - cam.pos;
    fwd.y = 0.0f;
    const bool hasFwd = fwd.length() > 1e-3f;
    if (hasFwd) fwd = fwd.normalized();
    const float behindLimit = TILE_SIZE * 0.7072f;
    auto fullyBehind = [&](const Vec3 &c) {
        if (!hasFwd) return false;
        Vec3 d = c - cam.pos;
        return d.x * fwd.x + d.z * fwd.z < -behindLimit;
    };

    // Pass 1: opaque tile batches, nearest tile first so the depth test
    // rejects hidden pixels before they are shaded.
    struct NearTile { int col, row; float d2; };
    NearTile near[(2 * kMaxDrawDistance + 1) * (2 * kMaxDrawDistance + 1)];
    int nearCount = 0;
    for (int row = std::max(1, pr - dist); row <= std::min(GRID, pr + dist); ++row)
        for (int col = std::max(1, pc - dist); col <= std::min(GRID, pc + dist); ++col) {
            Vec3 d = tileCenter(col, row) - cam.pos;
            near[nearCount++] = {col, row, d.x * d.x + d.z * d.z};
        }
    std::sort(near, near + nearCount,
              [](const NearTile &a, const NearTile &b) { return a.d2 < b.d2; });
    for (int i = 0; i < nearCount; ++i) {
        const int col = near[i].col, row = near[i].row;
        int idx = (col - 1) + (row - 1) * GRID;
        int id = kTileID[idx];
        if (id <= 0 || id > 37) continue;
        Vec3 center = tileCenter(col, row);
        if (fullyBehind(center)) continue;
        Mat4 xf = Mat4::translation(center) *
                  Mat4::rotationY(deg2rad(-90.0f * float(kTileRot[idx])));
        for (const TileBatch &b : protos[size_t(id)].batches) {
            if (b.blend || b.ranges.empty()) continue;
            const AssetMaterial &mat = gAssets.materials[size_t(b.material)];
            const Texture *tex = gAssets.texture(b.material);
            r.setSurface(mat.lighting);
            r.setUVOffset(mat.du, mat.dv);
            for (const TileRange &range : b.ranges) {
                if (!range.mesh || range.indexCount == 0) continue;
                r.drawMesh(cam, xf, *range.mesh, tex, mat.tint(),
                           false, true, mat.opacity, true,
                           range.indexBegin, range.indexCount);
            }
        }
    }

    r.resetSurface();
    r.setUVOffset(0.0f, 0.0f);

    // Opaque props.
    for (const WorldProp &p : props) {
        if (p.mesh < 0 || p.alpha < 0.999f) continue;
        const AssetMesh &m = gAssets.meshes[size_t(p.mesh)];
        bool transparent = false;
        for (const AssetGroup &g : m.groups)
            if (g.material >= 0 && gAssets.materials[size_t(g.material)].blend) transparent = true;
        if (transparent) continue;
        drawAssetMesh(r, cam, p.model, m, p.unlit, 1.0f, true);
    }

    // The original Landscape_TempSkyBox_000 follows the player. It is drawn
    // after the opaque world (same image, the depth test hides it behind
    // geometry) so the pixels the world covers are never shaded twice.
    if (const AssetMesh *sky = gAssets.mesh("Landscape_TempSkyBox_000")) {
        r.setFog(packRGBA(176, 203, 226), 1.0e8f, 2.0e8f);
        Mat4 m = Mat4::translation({cam.pos.x, 0.0f, cam.pos.z});
        drawAssetMesh(r, cam, m, *sky, true, 1.0f, false);
        r.setWorldFog();
    }

    // Pass 2: baked shadow planes and other alpha material batches.
    for (int row = std::max(1, pr - dist); row <= std::min(GRID, pr + dist); ++row) {
        for (int col = std::max(1, pc - dist); col <= std::min(GRID, pc + dist); ++col) {
            int idx = (col - 1) + (row - 1) * GRID;
            int id = kTileID[idx];
            if (id <= 0 || id > 37) continue;
            Vec3 center = tileCenter(col, row);
            if (fullyBehind(center)) continue;
            Mat4 xf = Mat4::translation(center) *
                      Mat4::rotationY(deg2rad(-90.0f * float(kTileRot[idx])));
            for (const TileBatch &b : protos[size_t(id)].batches) {
                if (!b.blend || b.ranges.empty()) continue;
                const AssetMaterial &mat = gAssets.materials[size_t(b.material)];
                const Texture *tex = gAssets.texture(b.material);
                r.setSurface(mat.lighting);
                r.setUVOffset(mat.du, mat.dv);
                for (const TileRange &range : b.ranges) {
                    if (!range.mesh || range.indexCount == 0) continue;
                    r.drawMesh(cam, xf, *range.mesh, tex, mat.tint(),
                               false, false, mat.opacity, false,
                               range.indexBegin, range.indexCount);
                }
            }
        }
    }

    r.setUVOffset(0.0f, 0.0f);
    ferris.draw(r, cam);
    race.draw(r, cam);
    colorTank.draw(r, cam);
    stage.draw(r, cam);
    traffic.draw(r, cam);
    plane.draw(r, cam);
    // doSetupTallObjects: mRadar at (3500, 0, 13500), spun by doAnims.
    if (radarMesh >= 0)
        drawAssetMesh(r, cam,
                      Mat4::translation({3500, 0, 13500}) * Mat4::rotationY(deg2rad(radarAngle)),
                      gAssets.meshes[size_t(radarMesh)]);

    // igNitro pickups: rotation (0, 180, 120), scale 2.
    if (nitroMesh >= 0) {
        for (const NitroPickup &p : nitroPickups) {
            if (!p.visible) continue;
            Mat4 m = Mat4::translation(p.pos) * Mat4::rotationZ(deg2rad(120.0f)) *
                     Mat4::rotationY(deg2rad(180.0f)) * Mat4::scale({2.0f, 2.0f, 2.0f});
            drawAssetMesh(r, cam, m, gAssets.meshes[size_t(nitroMesh)], false, 1.0f, true);
        }
    }

    for (const WorldProp &p : props) {
        if (p.mesh < 0 || p.alpha >= 0.999f) continue;
        drawAssetMesh(r, cam, p.model, gAssets.meshes[size_t(p.mesh)], p.unlit, p.alpha, true);
    }
}

Vec3 World::rixDoorPos() { return kRixDoorPos; }

void World::resolveCarCollision(Car &car) const {
    // doCollisions(): the original radial pushes for the four nearest tiles.
    resolveTileCollisions(*this, car);
}

void World::doCollisions(Car &car) {
    ++ticks;
    // doTraffic and doTrafficLights run first in enterFrame; nothing between
    // them and doCollisions moves the player, so running them here is the
    // same order.
    if (ready && objectsReady) {
        traffic.update(*this, car.pos, ticks);
        traffic.updateLights(ticks);
        traffic.collide(car);
    }
    resolveTileCollisions(*this, car);
}

// doRayCasting(): twelve downward rays from gCageList (doSetupPlayerCar). The
// four lowest points are the wheels; every embedded point adds an upward
// impulse and a pitch/roll torque, which is what makes the chassis ride on a
// spring and wobble after an impact.
void World::updateGround(Car &car) const {
    // Safety net for leaving the 12 x 12 map; the original is fenced in by the
    // DistantHill and AirportFence tiles.
    // The original 12 x 12 map covers [0, 60000] on both axes (tile centres
    // run from 2500 to 57500).
    const float minX = 0.0f, maxX = 60000.0f;
    const float minZ = 0.0f, maxZ = 60000.0f;
    if (car.pos.x < minX) { car.pos.x = minX; if (car.vel.x < 0) car.vel.x = 0; }
    if (car.pos.x > maxX) { car.pos.x = maxX; if (car.vel.x > 0) car.vel.x = 0; }
    if (car.pos.z < minZ) { car.pos.z = minZ; if (car.vel.z < 0) car.vel.z = 0; }
    if (car.pos.z > maxZ) { car.pos.z = maxZ; if (car.vel.z > 0) car.vel.z = 0; }

    if (!ready) {
        car.hoist = 0.0f;
        if (car.pos.y <= 0.0f) {
            car.pos.y = 0.0f;
            if (car.vel.y < 0.0f) car.vel.y = 0.0f;
            car.midAir = false;
        } else {
            car.midAir = true;
        }
        return;
    }

    // The cage is a child of mPlayerCar, so it inherits the body rotation.
    const Mat4 &body = car.orient;

    float imbed[12] = {};
    float cap[12] = {};
    float surface[12] = {};
    Vec3 toCenter[12];
    Vec3 normal[12];
    float hoist = 0.0f;
    bool grounded = false;

    for (int i = 0; i < 12; ++i) {
        Vec3 local = body.transformVector(kCage[i]);
        Vec3 p = car.pos + local;
        // The ray starts 60 above the cage point and goes straight down.
        float reach = p.y + 60.0f - 140.0f;
        // modelsUnderRay hits the real tile mesh, so read the height field
        // blended between cells; nearest-cell steps kick the springs.
        toCenter[i] = local;
        float y;
        if (!groundRaySmooth(p.x, p.z, reach, y)) {
            // tempResult.count = 0: the point adds nothing this step.
            surface[i] = p.y;
            continue;
        }
        float pen = y - p.y + 30.0f;
        surface[i] = y;
        if (pen > 0.0f) {
            grounded = true;
            imbed[i] = pen;
            cap[i] = pen > 20.0f ? 20.0f : pen;
            if (pen > 20.0f && hoist < pen - 20.0f) hoist = pen - 20.0f;
            normal[i] = groundNormalAt(p.x, p.z, reach);
        }
    }

    car.midAir = !grounded;
    car.hoist = hoist;
    for (int i = 0; i < 4; ++i) car.wheelImbed[i] = imbed[i];

    // Wheels: support impulse, torque, and a nudge along the surface normal
    // (downhill on slopes). The lever arms are toCenterList, the world-space
    // cage offsets, matching rotVector's world axes.
    for (int i = 0; i < 4; ++i) {
        car.vel.y += cap[i] * 0.045f;
        car.rotXDeg -= toCenter[i].z * cap[i] * 0.0008f;
        car.rotZDeg += toCenter[i].x * cap[i] * 0.0008f;
        if (imbed[i] > 0.0f) {
            car.vel.x += normal[i].x * 0.1f;
            car.vel.z += normal[i].z * 0.1f;
        }
    }
    // Upper cage points: weaker torque plus the velocity/rotation coupling
    // that lets the shell rock on kerbs and landings.
    for (int i = 4; i < 12; ++i) {
        car.rotXDeg -= toCenter[i].z * cap[i] * 0.0003f;
        car.rotZDeg += toCenter[i].x * cap[i] * 0.0003f;
        if (cap[i] > 0.0f) {
            car.vel.x -= car.rotZDeg * 0.06f;
            car.vel.z += car.rotXDeg * 0.06f;
            car.rotXDeg -= car.vel.z * 0.01f;
            car.rotZDeg -= car.vel.x * 0.01f;
        }
    }

    // Geometric slope under the four wheels, for --probe / --slope-scan.
    float frontH = (surface[2] + surface[3]) * 0.5f;
    float rearH = (surface[0] + surface[1]) * 0.5f;
    float leftH = (surface[0] + surface[2]) * 0.5f;
    float rightH = (surface[1] + surface[3]) * 0.5f;
    if (grounded) {
        car.groundPitchDeg = std::atan2(rearH - frontH, 120.0f) * 180.0f / PI;
        car.groundRollDeg = std::atan2(leftH - rightH, 100.0f) * 180.0f / PI;
    } else {
        car.groundPitchDeg = 0.0f;
        car.groundRollDeg = 0.0f;
    }

    car.groundY = surface[0];
    car.useGroundMap = true;

    if (std::getenv("RR4_DEBUG_GROUND")) {
        std::fprintf(stderr,
                     "ground pos=(%.1f,%.2f,%.1f) vel=(%.2f,%.2f,%.2f) "
                     "pitch=%.2f roll=%.2f rotX=%.2f rotZ=%.2f hoist=%.2f air=%d "
                     "cap=(%.2f,%.2f,%.2f,%.2f) tz=(%.1f,%.1f,%.1f,%.1f)\n",
                     car.pos.x, car.pos.y, car.pos.z, car.vel.x, car.vel.y,
                     car.vel.z, car.pitchDeg(), car.rollDeg(), car.rotXDeg,
                     car.rotZDeg, hoist, car.midAir ? 1 : 0,
                     cap[0], cap[1], cap[2], cap[3],
                     toCenter[0].z, toCenter[1].z, toCenter[2].z, toCenter[3].z);
    }
}

// Surface normal of the tile height field, for the doRayCasting slide term.
Vec3 World::groundNormalAt(float x, float z, float nearY) const {
    const float e = 40.0f;
    // Samples the ray misses (past the edge of a sunken floor) count as level
    // with the centre, so a floor edge does not read as a cliff.
    float hc;
    if (!groundRaySmooth(x, z, nearY, hc)) return Vec3(0, 1, 0);
    auto h = [&](float sx, float sz) {
        float y;
        return groundRaySmooth(sx, sz, nearY, y) ? y : hc;
    };
    float hl = h(x - e, z);
    float hr = h(x + e, z);
    float hd = h(x, z - e);
    float hu = h(x, z + e);
    Vec3 n{hl - hr, 2.0f * e, hd - hu};
    float m = n.length();
    return m > 1e-6f ? n / m : Vec3(0, 1, 0);
}

} // namespace rr4
