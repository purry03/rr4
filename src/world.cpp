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
    0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3,
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 0,
    2, 0, 0, 2, 1, 0, 0, 2, 0, 0, 0, 2,
    1, 1, 2, 3, 1, 1, 0, 2, 3, 2, 3, 3,
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
        uint32_t base = uint32_t(b.mesh.verts.size());
        b.mesh.verts.insert(b.mesh.verts.end(), src.verts.begin(), src.verts.end());
        for (uint32_t i = 0; i < g.count; ++i)
            b.mesh.idx.push_back(base + src.idx[g.start + i]);
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
        const AssetMesh *base = kBaseName[id] ? gAssets.mesh(kBaseName[id]) : nullptr;
        if (!base) continue;
        auto insert = [](HeightCell &c, float y) {
            for (int i = 0; i < 4; ++i)
                if (std::fabs(c.y[i] - y) < 1.0f) return;
            if (y <= c.y[3]) return;
            int p = 3;
            while (p > 0 && c.y[p - 1] < y) { c.y[p] = c.y[p - 1]; --p; }
            c.y[p] = y;
        };
        for (size_t t = 0; t + 2 < base->idx.size(); t += 3) {
            const Vec3 &a = base->verts[base->idx[t]].p;
            const Vec3 &b = base->verts[base->idx[t + 1]].p;
            const Vec3 &c = base->verts[base->idx[t + 2]].p;
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
    }
}

float World::groundHeightAt(float x, float z, float nearY) const {
    int idx = tileIndexAt(x, z);
    if (idx < 0) return 0.0f;
    int id = kTileID[idx];
    if (id <= 0 || id > 37 || heights[size_t(id)].empty()) return 0.0f;

    Vec3 center = tileCenter(idx % GRID + 1, idx / GRID + 1);
    Vec3 d{x - center.x, 0.0f, z - center.z};
    Mat4 inv = Mat4::rotationY(deg2rad(90.0f * float(kTileRot[idx])));
    Vec3 local = inv.transformVector(d);

    int gx = int((local.x + 2500.0f) / TILE_SIZE * HMAP);
    int gz = int((local.z + 2500.0f) / TILE_SIZE * HMAP);
    gx = std::max(0, std::min(HMAP - 1, gx));
    gz = std::max(0, std::min(HMAP - 1, gz));

    const HeightCell &c = heights[size_t(id)][size_t(gz) * HMAP + gx];
    float best = -1e30f, lowest = 1e30f;
    for (int i = 0; i < 4; ++i) {
        float y = c.y[i];
        if (y <= -1e29f) continue;
        lowest = std::min(lowest, y);
        if (y <= nearY + 140.0f && y > best) best = y;
    }
    if (best <= -1e29f) best = (lowest < 1e29f) ? lowest : 0.0f;
    return best;
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
    add("AirPortRadar_000", Mat4::translation({3500, 0, 13500}));
    add("TALLOBJECT_OuterWalls_000", Mat4::identity());
    // Static airliner parked at the airport end of the runway.
    add("TrafficBoeing747_001", Mat4::translation({7500, 0, 7500}));
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

    // The original Landscape_TempSkyBox_000 follows the player.
    if (const AssetMesh *sky = gAssets.mesh("Landscape_TempSkyBox_000")) {
        r.setFog(packRGBA(176, 203, 226), 1.0e8f, 2.0e8f);
        Mat4 m = Mat4::translation({cam.pos.x, 0.0f, cam.pos.z});
        drawAssetMesh(r, cam, m, *sky, true, 1.0f, false);
    }

    r.setFog(packRGBA(176, 203, 226), 9000.0f, 42000.0f);

    int pc = tileColAt(cam.pos.x);
    int pr = tileRowAt(cam.pos.z);
    const int dist = 2;

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

    // Pass 1: opaque tile batches.
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
                if (b.blend || b.mesh.idx.empty()) continue;
                const AssetMaterial &mat = gAssets.materials[size_t(b.material)];
                r.drawMesh(cam, xf, b.mesh, gAssets.texture(b.material), mat.color,
                           false, true, mat.opacity, true);
            }
        }
    }

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
                if (!b.blend || b.mesh.idx.empty()) continue;
                const AssetMaterial &mat = gAssets.materials[size_t(b.material)];
                r.drawMesh(cam, xf, b.mesh, gAssets.texture(b.material), mat.color,
                           false, false, mat.opacity, false);
            }
        }
    }

    for (const WorldProp &p : props) {
        if (p.mesh < 0 || p.alpha >= 0.999f) continue;
        drawAssetMesh(r, cam, p.model, gAssets.meshes[size_t(p.mesh)], p.unlit, p.alpha, true);
    }
}

void World::resolveCarCollision(Car &car) const {
    // doCollisions(): the original radial pushes for the four nearest tiles.
    resolveTileCollisions(*this, car);
}

// Surface contact, the original doRayCasting() part of enterFrame. Kept apart
// from resolveCarCollision so the collision pass can run mid-step (before
// doupdate) while ground snapping still runs after the car has moved.
void World::updateGround(Car &car) const {
    // Safety net for leaving the 12 x 12 map; the original is fenced in by the
    // DistantHill and AirportFence tiles.
    // The original 12 x 12 map covers [0, 60000] on both axes (tile centres
    // run from 2500 to 57500). Applied after the step so the clamp is final.
    const float minX = 0.0f, maxX = 60000.0f;
    const float minZ = 0.0f, maxZ = 60000.0f;
    if (car.pos.x < minX) { car.pos.x = minX; if (car.vel.x < 0) car.vel.x = 0; }
    if (car.pos.x > maxX) { car.pos.x = maxX; if (car.vel.x > 0) car.vel.x = 0; }
    if (car.pos.z < minZ) { car.pos.z = minZ; if (car.vel.z < 0) car.vel.z = 0; }
    if (car.pos.z > maxZ) { car.pos.z = maxZ; if (car.vel.z > 0) car.vel.z = 0; }

    // Follow the original tile surfaces (banked NurdBurger corners, bridge
    // decks, airport ramps). The original physics does the same with twelve
    // downward rays from the car cage.
    if (ready) {
        // doRayCasting fires twelve rays from gCageList. The four lowest cage
        // points (x = +-50, z = +-60, y = 30) carry the surface response:
        //   imbed = isectY - carY  (clamped to 0)
        //   rotVector.x -= toCenter.z * imbed * 0.0008
        //   rotVector.z += toCenter.x * imbed * 0.0008
        // A ray starts 60 above the cage point, so only surfaces within about
        // 90 units of the car can be hit. The height-field equivalent keeps
        // that reach limit and clamps to the same contact range, which keeps
        // overhead bridge decks and canal beds from twisting the shell.
        const CarConfig &cfg = carConfig(car.carId);
        Vec3 fwd = car.forward();
        Vec3 axisX = car.rightAxis(); // local +x; this is the car's left.
        float front = cfg.tirePivot.z;
        float rear = cfg.tirePivot.z + cfg.rearAxleOffset.z;
        float halfTrack = cfg.tirePivot.x + 7.5f;
        const float reach = 90.0f;
        const float nearY = car.pos.y - 50.0f;
        auto wheelHeight = [&](float lateral, float longitudinal) {
            Vec3 p = car.pos + fwd * longitudinal + axisX * lateral;
            float h = groundHeightAt(p.x, p.z, nearY);
            return clampf(h, car.pos.y, car.pos.y + reach);
        };
        float hFrontPosX = wheelHeight(halfTrack, front);   // front left
        float hFrontNegX = wheelHeight(-halfTrack, front);  // front right
        float hRearPosX = wheelHeight(halfTrack, rear);     // rear left
        float hRearNegX = wheelHeight(-halfTrack, rear);    // rear right
        float frontH = (hFrontPosX + hFrontNegX) * 0.5f;
        float rearH = (hRearPosX + hRearNegX) * 0.5f;
        float leftH = (hFrontPosX + hRearPosX) * 0.5f;
        float rightH = (hFrontNegX + hRearNegX) * 0.5f;

        // rotationX positive dips the nose (local +z maps to -y), so climbing
        // ground needs rearH - frontH. rotationZ positive lifts local +x,
        // which is the left side, so higher left ground needs leftH - rightH.
        float pitchTarget = std::atan2(rearH - frontH, std::max(1.0f, front - rear)) *
                            180.0f / PI;
        float rollTarget = std::atan2(leftH - rightH, std::max(1.0f, halfTrack * 2.0f)) *
                           180.0f / PI;
        pitchTarget = clampf(pitchTarget, -30.0f, 30.0f);
        rollTarget = clampf(rollTarget, -30.0f, 30.0f);
        if (car.midAir) {
            pitchTarget = 0.0f;
            rollTarget = 0.0f;
        }
        car.surfacePitchDeg += (pitchTarget - car.surfacePitchDeg) * 0.25f;
        car.surfaceRollDeg += (rollTarget - car.surfaceRollDeg) * 0.25f;

        // Support height still comes from under the car centre, so a wheel
        // over a kerb or canal edge cannot pop the whole body upwards.
        float g = groundHeightAt(car.pos.x, car.pos.z, nearY);
        if (car.pos.y < g) {
            car.pos.y = g;
            if (car.vel.y < -10.0f) car.landImpact = std::max(car.landImpact, -car.vel.y);
            if (car.vel.y < 0) car.vel.y = 0;
            car.midAir = false;
        } else if (car.pos.y - g < 160.0f && car.vel.y <= 0.0f) {
            car.pos.y = g;
            if (car.vel.y < -10.0f) car.landImpact = std::max(car.landImpact, -car.vel.y);
            car.vel.y = 0;
            car.midAir = false;
        } else {
            car.midAir = true;
        }
        car.groundY = g;
        car.useGroundMap = true;
    }
}

} // namespace rr4
