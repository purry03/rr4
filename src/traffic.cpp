#include "traffic.hpp"
#include "assets.hpp"
#include "car.hpp"
#include "world.hpp"

#include <cmath>
#include <strings.h>

namespace rr4 {

namespace {

constexpr int kSlots = 20;               // doSetupTraffic: 20 clones
constexpr float kSpawnDist = 4000.0f;    // trafficSpawnDist
constexpr float kCullDist = 4500.0f;     // trafficCullDist
constexpr int kRedHold = -1111;          // a red light's empty dot

// doTrafficLights drives two light sets. Each phase sets red, amber and green
// of both sets to rgb(255) when lit or rgb(55) when dim (emissive = diffuse).
struct LightShader { const char *name; Vec3 lit; };
const LightShader kLightShaders[6] = {
    {"Texture_Tile_TrafficLightPhase1Red", {1, 0, 0}},
    {"Texture_Tile_TrafficLightPhase1Amber", {1, 1, 0}},
    {"Texture_Tile_TrafficLightPhase1Green", {0, 1, 0}},
    {"Texture_Tile_TrafficLightPhase2Red", {1, 0, 0}},
    {"Texture_Tile_TrafficLightPhase2Amber", {1, 1, 0}},
    {"Texture_Tile_TrafficLightPhase2Green", {0, 1, 0}},
};
// Which of the six shaders are lit in phases 1..6, and how long each phase
// lasts in ticks.
const bool kLit[6][6] = {
    {true, false, false, false, false, true},    // 1: set 1 red, set 2 green
    {true, false, false, false, true, false},    // 2: set 2 amber
    {true, false, false, true, false, false},    // 3: all red
    {false, false, true, true, false, false},    // 4: set 1 green
    {false, true, false, true, false, false},    // 5: set 1 amber
    {true, false, false, true, false, false},    // 6: all red
};
const long kPhaseTicks[6] = {400, 100, 100, 400, 100, 100};
int gLightMaterial[6] = {-1, -1, -1, -1, -1, -1};

int findMaterial(const char *name) {
    for (size_t i = 0; i < gAssets.materials.size(); ++i)
        if (strcasecmp(gAssets.materials[i].name.c_str(), name) == 0) return int(i);
    return -1;
}

// Director pointAt with the default orientation: -Z faces `dir`, +Y up.
bool pointAtRotation(const Vec3 &dir, Mat4 &out) {
    if (dir.x * dir.x + dir.z * dir.z < 1e-6f) return false;
    Vec3 z = (dir * -1.0f).normalized();
    Vec3 x = Vec3(0, 1, 0).cross(z).normalized();
    Vec3 y = z.cross(x);
    out = Mat4::identity();
    out.m[0] = x.x; out.m[1] = x.y; out.m[2] = x.z;
    out.m[4] = y.x; out.m[5] = y.y; out.m[6] = y.z;
    out.m[8] = z.x; out.m[9] = z.y; out.m[10] = z.z;
    return true;
}

const Vec3 &dotPos(int id) { return kTrafficMap.dotPos[id - 1]; }
int nextCount(int id) { return kTrafficMap.nextStart[id] - kTrafficMap.nextStart[id - 1]; }
int nextDot(int id, int k) { return kTrafficMap.nextIds[kTrafficMap.nextStart[id - 1] + k - 1]; }
int prevCount(int id) { return kTrafficMap.prevStart[id] - kTrafficMap.prevStart[id - 1]; }
int prevDot(int id, int k) { return kTrafficMap.prevIds[kTrafficMap.prevStart[id - 1] + k - 1]; }

float flatDist(const Vec3 &a, const Vec3 &b) {
    Vec3 d = a - b;
    d.y = 0.0f;
    return d.length();
}

} // namespace

int Traffic::random(int n) {
    seed_ ^= seed_ << 13; seed_ ^= seed_ >> 17; seed_ ^= seed_ << 5;
    return 1 + int(seed_ % uint32_t(n));
}

void Traffic::setup() {
    mesh_ = gAssets.meshIndex("TrafficCar_000");
    slots_.assign(kSlots, Slot{});
    occupied_.assign(size_t(kTrafficMap.dotCount), 0);
    spawnTicks_.assign(size_t(kTrafficMap.dotCount), 0);
    spawned_ = 0;
    for (int i = 0; i < 6; ++i) gLightMaterial[i] = findMaterial(kLightShaders[i].name);
    // doSetupTrafficLights: phase 1 holds the red set.
    lightPhase_ = 1;
    for (int i = 0; i < kTrafficMap.redCount; ++i) {
        int &o = occupied(kTrafficMap.redLights[i]);
        o = o > 0 ? -o : kRedHold;
    }
    // trafficLightStartTicks starts unset (0) while the ticks count from
    // machine start, so the first phase ends straight away.
    lightStartTicks_ = -100000;
}

void Traffic::updateLights(long ticks) {
    // A light turning red keeps the car standing on its dot (stored
    // negated) and holds the empty ones; turning green releases both.
    auto hold = [&](const int *ids, int n) {
        for (int i = 0; i < n; ++i) {
            int &o = occupied(ids[i]);
            o = o > 0 ? -o : kRedHold;
        }
    };
    auto release = [&](const int *ids, int n) {
        for (int i = 0; i < n; ++i) {
            int &o = occupied(ids[i]);
            if (o < 0) o = o == kRedHold ? 0 : -o;
        }
    };

    const bool *lit = kLit[lightPhase_ - 1];
    for (int i = 0; i < 6; ++i) {
        if (gLightMaterial[i] < 0) continue;
        Vec3 c = lit[i] ? kLightShaders[i].lit : kLightShaders[i].lit * (55.0f / 255.0f);
        SurfaceLighting &l = gAssets.materials[size_t(gLightMaterial[i])].lighting;
        l.emissive = c;
        l.diffuse = c;
    }
    if (ticks - lightStartTicks_ <= kPhaseTicks[lightPhase_ - 1]) return;
    lightStartTicks_ = ticks;
    switch (lightPhase_) {
    case 1: hold(kTrafficMap.greenLights, kTrafficMap.greenCount); break;
    case 3: release(kTrafficMap.redLights, kTrafficMap.redCount); break;
    case 4: hold(kTrafficMap.redLights, kTrafficMap.redCount); break;
    case 6: release(kTrafficMap.greenLights, kTrafficMap.greenCount); break;
    default: break;
    }
    lightPhase_ = lightPhase_ % 6 + 1;
}

void Traffic::update(const World &world, const Vec3 &focus, long ticks) {
    // Candidate spawn dots: on the player's tile and its eight neighbours,
    // between 4000 and 4500 from the player, free, and not used for 200 ticks.
    const int col = world.tileColAt(focus.x), row = world.tileRowAt(focus.z);
    std::vector<int> spawnDots;
    // tempTileList order: own tile, then the eight neighbours row by row.
    static const int kAround[9][2] = {{0, 0}, {-1, -1}, {0, -1}, {1, -1}, {-1, 0},
                                      {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
    for (const auto &a : kAround) {
        int c = col + a[0], r = row + a[1];
        if (c < 1 || c > GRID || r < 1 || r > GRID) continue;
        int tile = c + (r - 1) * GRID;   // onTile, 1-based
        if (tile > kTrafficMap.tileCount) continue;
        for (int k = kTrafficMap.tileStart[tile - 1]; k < kTrafficMap.tileStart[tile]; ++k) {
            int dot = kTrafficMap.tileIds[k];
            float d = flatDist(focus, dotPos(dot));
            if (d < kCullDist && d > kSpawnDist && occupied(dot) == 0 &&
                ticks - spawnTicks_[size_t(dot - 1)] > 200)
                spawnDots.push_back(dot);
        }
    }

    for (int i = 1; i <= int(slots_.size()); ++i) {
        Slot &s = slots_[size_t(i - 1)];
        if (s.state == 0) {
            if (spawnDots.empty() || spawned_ >= spawnLimit_) continue;
            s.state = 1;
            int pick = random(int(spawnDots.size()));
            int dot = spawnDots[size_t(pick - 1)];
            spawnDots.erase(spawnDots.begin() + (pick - 1));
            s.pos = dotPos(dot);
            s.yaw = 0.0f;
            s.target = s.pos;
            s.dot = dot;
            occupied(dot) = i;
            ++spawned_;
            spawnTicks_[size_t(dot - 1)] = ticks;
            if (nextCount(dot) > 0) {
                Vec3 to = dotPos(nextDot(dot, 1));
                if (flatDist(s.pos, to) > 0.0f) s.yaw = pointAtYaw(s.pos, to);
                s.targetSpeed = (s.target - dotPos(nextDot(dot, 1))).length() * 0.02f;
            } else {
                s.targetSpeed = s.speed = 0.0f;
                if (prevCount(dot) > 0) {
                    Vec3 to = dotPos(prevDot(dot, 1));
                    if (flatDist(s.pos, to) > 0.0f) s.yaw = pointAtYaw(s.pos, to);
                }
            }
            // The car model sits turned 180 inside its group.
            s.body = Mat4::rotationY(s.yaw + PI);
            continue;
        }

        s.speed += (s.targetSpeed - s.speed) * 0.04f;
        const Vec3 prevPos = s.pos;
        Vec3 toCar = s.pos - s.target;
        toCar.y = 0.0f;
        if (toCar.length() < 100.0f) {
            // At the dot: claim a random next dot, or wait if it is taken
            // (another car or a red light).
            int n = nextCount(s.dot);
            int next = n > 0 ? nextDot(s.dot, random(n)) : 0;
            if (next > 0 && occupied(next) == 0) {
                int &cur = occupied(s.dot);
                if (std::abs(cur) == i) cur = cur < 0 ? kRedHold : 0;
                occupied(next) = i;
                s.targetSpeed = (s.target - dotPos(next)).length() * 0.02f;
                spawnTicks_[size_t(s.dot - 1)] = ticks;
                s.dot = next;
                s.target = dotPos(next);
            } else {
                s.targetSpeed = s.speed = 0.0f;
            }
        } else {
            // Steer 4.5 degrees per frame towards the target and drive on.
            Mat4 rot = Mat4::rotationY(s.yaw);
            float turn = toCar.normalized().dot(rot.transformVector({1, 0, 0})) * 4.5f;
            s.yaw += deg2rad(turn);
            s.pos += Mat4::rotationY(s.yaw).transformVector({0, 0, -s.speed});
            // modelsUnderRay from 100 above the car straight down.
            s.pos.y = world.groundHeightSmooth(s.pos.x, s.pos.z, s.pos.y - 40.0f);
        }
        // The model looks back at where it was, which also pitches it on slopes.
        Mat4 body;
        if (pointAtRotation(prevPos - s.pos, body)) s.body = body;

        if (flatDist(s.pos, focus) > kCullDist) {
            s.state = 0;
            occupied(s.dot) = 0;
            s.dot = 0;
            --spawned_;
        }
    }
}

void Traffic::collide(Car &player) const {
    const float radius = 140.0f;
    for (const Slot &s : slots_) {
        if (s.state != 1) continue;
        Vec3 diff = s.pos - (player.pos + player.vel);
        float mag = diff.length();
        if (mag >= radius || mag < 1e-4f) continue;
        Vec3 push = diff.normalized() * (radius - mag);
        player.vel -= push;
        player.rotZDeg -= push.x * 0.1f;
        player.rotXDeg += push.z * 0.1f;
        float pushMag = push.length();
        // CrashTinkle small (Sfx 16) above 5, medium (Sfx 18) above 15.
        if (pushMag > 5.0f) {
            player.damage += 0.002f;
            player.sfx.push_back(16);
        }
        if (pushMag > 15.0f) {
            player.damage += 0.005f;
            player.sfx.push_back(18);
        }
    }
}

void Traffic::draw(Renderer &r, const Camera &cam) const {
    if (mesh_ < 0) return;
    const AssetMesh &m = gAssets.meshes[size_t(mesh_)];
    for (const Slot &s : slots_) {
        if (s.state != 1) continue;
        // Cars spawn 4000..4500 from the player, inside the fog; fade them in
        // over that band so they do not pop.
        float fade = clampf((kCullDist - flatDist(s.pos, cam.pos)) / 1000.0f, 0.0f, 1.0f);
        if (fade <= 0.0f) continue;
        drawAssetMesh(r, cam, Mat4::translation(s.pos) * s.body, m, false, fade);
    }
}

} // namespace rr4
