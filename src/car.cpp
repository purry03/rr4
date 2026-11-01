#include "car.hpp"
#include "assets.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace rr4 {

namespace {

struct CarMeshes {
    int body = -1, bonnet = -1, boot = -1, door = -1, tire = -1;
};

CarMeshes gCarMeshes[6];

// Data from doSetupShells and doControls. Part pivots are the BONNET / BOOT /
// DOOR / TIRE node translations relative to their BODY parent, read from the
// node (0xffffff72) blocks of the original 3DEM stream (XMED-6650). Tire y is
// 16, which doAnims writes every frame (16 + imbedList). Rear axle offsets
// are the literal tempTireBL.translate values from doSetupShells.
const CarConfig kCars[6] = {
    {"MeanieCropper", "MeanieCropper_BODY_000", "MeanieCropper_BONNET_000",
     "MeanieCropper_BOOT_000", "MeanieCropper_DOOR_000", "MeanieCropper_TIRE_000",
     {0.0f, 58.13f, 53.21f}, {0.0f, 28.38f, -90.35f}, {46.07f, 3.91f, 47.96f},
     {33.01f, 16.0f, 73.69f}, {0.0f, 0.0f, -130.0f}, 0.0250f,
     4, 1,
     "Meanie_RevvingUp_001", "Meanie_RevvingDown_000", 1, 2,
     {0, 85, -80}, {30, 20, -95}, {-30, 20, -95}},
    {"Bitsasushi", "Bitsasushi_BODY_000", "Bitsasushi_BONNET_000",
     "Bitsasushi_BOOT_000", "Bitsasushi_DOOR_000", "Bitsasushi_TIRE_000",
     {0.0f, 56.01f, 57.99f}, {0.0f, 60.00f, -78.00f}, {48.00f, 40.03f, 47.97f},
     {35.29f, 16.0f, 76.27f}, {2.0f, 0.0f, -150.0f}, 0.0325f,
     10, 2,
     "Bitsasushi_RevvingUp_000", "Bitsasushi_RevvingDown_001", 3, 4,
     {0, 96, -110}, {-30, 25, -125}, {30, 25, -125}},
    {"Amazed", "Amazed_BODY_012", "Amazed_BONNET_000",
     "Amazed_BOOT_000", "Amazed_DOOR_000", "Amazed_TIRE_000",
     {0.0f, 54.91f, 42.02f}, {0.0f, 58.41f, -90.62f}, {51.00f, 39.90f, 40.99f},
     {38.30f, 16.0f, 84.39f}, {2.0f, 0.0f, -160.0f}, 0.0350f,
     1, 3,
     "Amazed_RevvingUp_000", "Amazed_RevvingDown_003", 5, 6,
     {0, 92, -110}, {-38, 25, -125}, {38, 25, -125}},
    {"HighWire", "HighWire_BODY_000", "HighWire_BONNET_000",
     "HighWire_BOOT_000", "HighWire_DOOR_000", "HighWire_TIRE_000",
     {0.0f, 55.26f, 51.60f}, {0.0f, 58.82f, -66.85f}, {49.98f, 1.02f, 48.20f},
     {38.15f, 16.0f, 77.53f}, {2.0f, 0.0f, -134.0f}, 0.0375f,
     10, 4,
     "skyline_RevvingUp_000", "skyline_RevvingDown_000", 7, 8,
     {0, 94, -95}, {-30, 22, -120}, {30, 22, -120}},
    {"Posher", "Posher_BODY_007", "Posher_BONNET_000",
     "Posher_BOOT_000", "Posher_DOOR_000", "Posher_TIRE_000",
     {0.0f, 51.27f, 44.24f}, {0.0f, 59.45f, -94.42f}, {54.50f, 0.00f, 46.49f},
     {36.99f, 16.0f, 78.00f}, {2.0f, 0.0f, -150.0f}, 0.0400f,
     2, 5,
     "Posher_RevvingUp_000", "Posher_RevvingDown_001", 9, 10,
     {0, 92, -120}, {-36, 20, -140}, {36, 20, -140}},
    {"Furore", "Furore_BODY_000", "FuroreI_BONNET_000",
     "FuroreI_BOOT_000", "FuroreI_DOOR_000", "FuroreI_TIRE_000",
     {0.0f, 51.21f, 54.00f}, {0.0f, 74.51f, -55.00f}, {52.00f, 31.98f, 46.00f},
     {38.31f, 16.0f, 75.86f}, {2.0f, 0.0f, -150.0f}, 0.0425f,
     5, 1,
     "Furore_RevvingUp_000", "Furore_RevvingDown_002", 11, 12,
     {0, 105, -70}, {-45, 25, -120}, {45, 25, -120}},
};

Texture gFallbackTex;
Texture gBurnSmokeTex;   // Textures cast member 6, texture_BurnSmoke_000
int gNitroBottle = -1;   // igNitroBottle_001 (mIgNitro, the boot bottle)
int gNitroBlast = -1;    // igNitroBlastJet_000

uint32_t nextRand(uint32_t &s) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
float rand01(uint32_t &s) { return float(nextRand(s) >> 8) * (1.0f / 16777216.0f); }

// Spawn point doDamageMaxOutReset moves the wrecked car to.
const Vec3 kRepairSpawn{33650.14f, -5.56f, 33374.08f};

} // namespace

int carCount() { return 6; }

const CarConfig &carConfig(int id) {
    return kCars[size_t(std::clamp(id, 1, carCount()) - 1)];
}

void Car::setCar(int id) { carId = std::clamp(id, 1, carCount()); }

void buildCarAssets() {
    for (int i = 0; i < carCount(); ++i) {
        const CarConfig &cfg = kCars[i];
        CarMeshes &m = gCarMeshes[i];
        m.body = gAssets.meshIndex(cfg.body);
        m.bonnet = gAssets.meshIndex(cfg.bonnet);
        m.boot = gAssets.meshIndex(cfg.boot);
        m.door = gAssets.meshIndex(cfg.door);
        m.tire = gAssets.meshIndex(cfg.tire);
        bool ok = m.body >= 0 && m.bonnet >= 0 && m.boot >= 0 &&
                  m.door >= 0 && m.tire >= 0;
        std::printf("car %d %-14s meshes %s (body %d, bonnet %d, boot %d, door %d, tire %d)\n",
                    i + 1, cfg.name, ok ? "loaded" : "MISSING", m.body, m.bonnet,
                    m.boot, m.door, m.tire);
        if (!ok) continue;

        // The original repaints the shell by swapping the shader's texture
        // for a 2 x 2 paint bitmap (Textures members 151..158, extracted to
        // assets/hud/PaintN.png). The paint is opaque on the car.
        const AssetMesh &body = gAssets.meshes[size_t(m.body)];
        if (cfg.paintShader >= 1 && cfg.paintShader <= int(body.groups.size())) {
            int matIndex = body.groups[size_t(cfg.paintShader - 1)].material;
            Texture paint;
            char path[64];
            std::snprintf(path, sizeof(path), "assets/hud/Paint%d.png", cfg.paintColor);
            if (matIndex >= 0 && loadTexture(path, paint)) {
                for (uint32_t &px : paint.px) px |= 0xff000000u;
                paint.buildMips();
                gAssets.textures.push_back(std::move(paint));
                AssetMaterial &mat = gAssets.materials[size_t(matIndex)];
                mat.texture = int(gAssets.textures.size()) - 1;
                mat.blend = mat.opacity < 0.999f;
            }
        }
    }

    if (loadTexture("assets/hud/BurnSmoke.png", gBurnSmokeTex)) gBurnSmokeTex.buildMips();
    gNitroBottle = gAssets.meshIndex("igNitroBottle_001");
    gNitroBlast = gAssets.meshIndex("igNitroBlastJet_000");

    // Fallback only used when the original textures are absent from a pack.
    gFallbackTex.w = gFallbackTex.h = 2;
    gFallbackTex.px = {packRGBA(214, 92, 40), packRGBA(214, 92, 40),
                       packRGBA(214, 92, 40), packRGBA(214, 92, 40)};
}

void Car::update(const CarInput &in, float factor,
                 const std::function<void(Car &)> &ground,
                 const std::function<void(Car &)> &collide) {
    const bool wasAir = midAir;
    landImpact = 0.0f;
    impactSpeed = 0.0f;
    respawned = false;
    repairSfx = false;
    nitroPickedUp = false;
    ferrisCreakSfx = ferrisTurnSfx = false;
    raceReturn = false;
    panels.clear();
    sfx.clear();
    fadeToBlack = false;
    timerState = kTimerUnchanged;
    timerReset = false;
    hoist = 0.0f;

    // ---- doRayCasting -----------------------------------------------------
    // Twelve downward rays from the car cage. Each embedded point adds an
    // upward velocity impulse and a pitch/roll torque, so the chassis rides a
    // spring over the tiles and keeps rocking after an impact.
    if (ground) ground(*this);

    // ---- doControls -------------------------------------------------------
    if (in.left) {
        if (steer < 0) steer = 0;
        if (steer < 40) steer += 2.0f * factor;
    }
    if (in.right) {
        if (steer > 0) steer = 0;
        if (steer > -40) steer -= 2.0f * factor;
    }
    if (!in.left && !in.right) {
        if (steer > 0) steer = std::max(0.0f, steer - 5.0f * factor);
        if (steer < 0) steer = std::min(0.0f, steer + 5.0f * factor);
    }
    // keyPressedList[4] (Up) raises throttle, keyPressedList[3] (Down) lowers it.
    // A wrecked car (damage >= 1) keeps its throttle but cannot raise it.
    if (in.up && !in.down) {
        if (throttle < 0) throttle = 0;
        if (damage < 1.0f) throttle += 0.2f * factor;
    }
    if (in.down && !in.up) {
        if (throttle > 0) throttle = 0;
        if (damage < 1.0f) throttle -= 0.2f * factor;
    }
    if (!in.up && !in.down) throttle = 0;
    throttle = clampf(throttle, -8.0f, 10.0f);

    Vec3 fwd = forward();
    Vec3 right = rightAxis();

    // doControls case playerCarID: car #1 = 0.025, #6 = 0.0425.
    if (!midAir) {
        if (damage < 1.0f) vel += fwd * (throttle * carConfig(carId).accel * factor);
        if (in.boost && boostMeter > 0.0f && !parked) {
            vel += fwd * (throttle * 0.025f * factor);
            boostMeter = std::max(0.0f, boostMeter - 0.005f * factor);
        }
    }

    // tempFactor = -zAxis . vel; rotVector.y -= steerVel, but only while a
    // front cage ray is embedded (imbedList[1] or [2] > 0).
    float fwdSpeed = fwd.dot(vel);
    float steerVel = clampf(steer * 0.001f * fwdSpeed, -0.5f, 0.5f);
    if (!midAir) yawVelDeg += steerVel * factor;

    // ---- doGravity --------------------------------------------------------
    vel.y -= 1.0f * factor;

    // ---- doFriction -------------------------------------------------------
    if (!midAir) {
        vel.y -= vel.y * 0.05f * factor;
        vel -= vel * (0.005f * factor);
        yawVelDeg -= yawVelDeg * (0.16f * factor);
        rotXDeg -= rotXDeg * (0.16f * factor);
        rotZDeg -= rotZDeg * (0.16f * factor);
    }

    // ---- doTireGrip -------------------------------------------------------
    float lat = vel.dot(right);
    Vec3 grip = right * lat;
    if (grip.length() > 1.0f * factor) grip = grip.normalized() * (1.0f * factor);
    if (!midAir) vel -= grip;

    // ---- doCollisions -----------------------------------------------------
    // Runs before doBodyRoll and doupdate. The radial pushes act on this
    // frame's velocity while the car still sits at its pre-move position, so
    // doupdate() then carries the pushed velocity out of the obstacle. If the
    // pass ran after the move, the push would be applied to the next frame and
    // the car would visibly bounce off walls every frame.
    if (collide) collide(*this);

    // ---- doBodyRoll -------------------------------------------------------
    // diffVector = velVector - prevVelVector, horizontal, capped at 1.
    Vec3 diff = vel - prevVel_;
    diff.y = 0.0f;
    if (diff.length() > 1.0f) diff = diff.normalized();
    rotXDeg -= diff.z * 0.35f * factor;
    rotZDeg += diff.x * 0.35f * factor;
    prevVel_ = vel;

    // ---- doupdate ---------------------------------------------------------
    // rotVector is an angular velocity: the body rotation accumulates it.
    pos += vel * factor + Vec3(0.0f, hoist * factor, 0.0f);
    bodyPitchDeg += rotXDeg * factor;
    bodyRollDeg += rotZDeg * factor;
    yaw += deg2rad(yawVelDeg * factor);
    if (yaw > PI) yaw -= 2 * PI;
    if (yaw < -PI) yaw += 2 * PI;

    // doupdate: damage maxing out starts a 300 tick (5 s) countdown, then
    // doDamageMaxOutReset repairs the car at Rix Rapid Repairs.
    // During a race the race fails (panel 30, raceState 7); in free drive
    // the "Car Wrecked!" panel 31 shows.
    if (damage >= 1.0f && prevDamage_ < 1.0f) {
        wreckTicks_ = 0.0f;
        if (inRace) {
            doPanel(30, 200);
            raceWrecked = true;
            timerReset = true;
            timerState = 0;
        } else {
            doPanel(31, 300);
        }
    }
    if (wreckTicks_ >= 0.0f) {
        wreckTicks_ += factor;
        if (wreckTicks_ > 300.0f) damageMaxOutReset();
    }

    // Flat-floor fallback for the standalone physics trace; World::updateGround
    // owns ground contact whenever it is driving the car.
    if (!useGroundMap) {
        if (pos.y <= 0.0f) {
            pos.y = 0.0f;
            if (wasAir && vel.y < 0.0f) landImpact = -vel.y;
            if (vel.y < 0) vel.y = 0;
            midAir = false;
        } else {
            midAir = true;
        }
    }

    // Tile collisions run mid-step through the `collide` hook above; ground
    // contact and the map edge fence follow in World::updateGround().

    // ---- doAnims (tire rotation) -----------------------------------------
    // rollVel = normalized(vel*1.6) . zAxis * |vel*1.6|, then the original
    // adds throttle * 2 before rotating the tires.
    Vec3 rollVec = vel * 1.6f;
    rollVec.y = 0.0f;
    float rollFactor = rollVec.length();
    float rollVel = 0.0f;
    if (rollFactor > 1e-4f) rollVel = (rollVec / rollFactor).dot(fwd) * rollFactor;
    wheelSpin += (rollVel + throttle * 2.0f) * factor;

    doParts(in, factor);
    doBurnSmoke(factor);
    doNitroBlast(in, factor);

    speedKmh = std::fabs(fwdSpeed) * 60.0f * 0.036f;
    partsPrevVel_ = vel;
    // Overlays.ls exitFrame caps g_DamagePercent at 1 every frame.
    damage = std::min(damage, 1.0f);
    prevDamage_ = damage;
}

void Car::damageMaxOutReset() {
    damage = 0.0f;
    boostMeter = 1.0f;
    pos = kRepairSpawn;
    yaw = 0.0f;                       // mPlayerCar.transform.rotation = (0, 0, 0)
    bodyPitchDeg = bodyRollDeg = 0.0f;
    vel = {};
    yawVelDeg = rotXDeg = rotZDeg = 0.0f;
    resetParts();
    wreckTicks_ = -1.0f;
    respawned = true;
    doPanel(32, 400);
    fadeToBlack = true;
}

// doParts / the partStateList thresholds in doAnims. Damaged panels come loose
// on their hinges and swing with the car's motion: bonnet above 20 % damage,
// left door above 40 %, right door above 60 %, boot above 80 %. States 2..4
// are empty in the original script, so panels never detach.
void Car::doParts(const CarInput &in, float factor) {
    const int loose = damage > 0.8f ? 4 : damage > 0.6f ? 3 : damage > 0.4f ? 2
                                                          : damage > 0.2f ? 1 : 0;
    const float diffY = partsPrevVel_.y - vel.y;
    const float damp = 0.02f * factor;

    // Swing toward a hinge stop and bounce off it at 60 % speed.
    auto swing = [&](int i, float accel, float lo, float hi) {
        partRotVel[i] += accel;
        partRotVel[i] -= partRotVel[i] * damp;
        partRot[i] += partRotVel[i] * factor;
        if (partRot[i] > hi) { partRot[i] = hi; partRotVel[i] = -partRotVel[i] * 0.6f; }
        if (partRot[i] < lo) { partRot[i] = lo; partRotVel[i] = -partRotVel[i] * 0.6f; }
    };
    if (loose > 0)   // bonnet
        swing(0, (diffY - partRot[0] * 0.01f - throttle * 0.01f) * factor, -60.0f, 0.0f);
    if (loose > 1)   // left door
        swing(1, (diffY + yawVelDeg * 0.1f + throttle * 0.01f - partRot[1] * 0.001f) * factor,
              -80.0f, 0.0f);
    if (loose > 2)   // right door
        swing(2, (diffY - yawVelDeg * 0.1f + throttle * 0.01f - partRot[2] * 0.001f) * factor,
              -80.0f, 0.0f);
    if (loose > 3)   // boot
        swing(3, (diffY - partRot[3] * 0.01f - throttle * 0.01f) * factor, 0.0f, 60.0f);
    // The boot also lifts to 30 while the nitro fires or the boot bottle is
    // still flipping after a pickup (igNitroRot > 0), loose or not.
    const float bootTarget =
        ((in.boost && boostMeter > 0.0f) || igNitroRot > 0.0f) ? 30.0f : 0.0f;
    partRot[3] -= (partRot[3] - bootTarget) * 0.1f * factor;
}

// doAnims igNitroBlastJet: while Shift is held with boost left the jets fade
// in (+5 blend per frame) and stretch to scale.z 1; afterwards they fade out
// and shrink (the right jet follows the left one's scale, as in the script).
// Each frame both spin to a random rotation.z.
void Car::doNitroBlast(const CarInput &in, float factor) {
    blastSpinL = float(1 + nextRand(smokeSeed_) % 360);
    blastSpinR = float(1 + nextRand(smokeSeed_) % 360);
    if (in.boost && boostMeter > 0.0f && !parked) {
        if (blastBlend < 100) blastBlend += 5;
        blastScaleL -= (blastScaleL - 1.0f) * 0.2f * factor;
        blastScaleR = blastScaleL - (blastScaleL - 1.0f) * 0.2f * factor;
    } else if (blastBlend >= 5) {
        blastBlend -= 5;
        blastScaleL -= (blastScaleL - 0.2f) * 0.2f * factor;
        blastScaleR = blastScaleL - (blastScaleL - 0.9f) * 0.2f * factor;
    }
}

// doAnims moves the burn smoke emitter in front of the car while damage is at
// least 60 %; doSetupBurnSmoke: 64 particles over a 1000 ms lifetime, 300..360
// units/s inside a 20 degree cone around +Y, gravity (0, -1, 0), drag 1.
void Car::doBurnSmoke(float factor) {
    const float dt = factor / 60.0f;
    for (SmokeParticle &p : smoke) {
        p.vel.y -= 1.0f * dt;
        p.vel -= p.vel * (0.01f * factor);
        p.pos += p.vel * dt;
        p.age += dt;
    }
    std::erase_if(smoke, [](const SmokeParticle &p) { return p.age >= 1.0f; });

    if (damage < 0.6f || parked) { smokeEmit_ = 0.0f; return; }
    const Vec3 origin = pos + vel + forward() * 105.0f;
    smokeEmit_ += 64.0f * dt;
    for (; smokeEmit_ >= 1.0f && smoke.size() < 64; smokeEmit_ -= 1.0f) {
        float a = deg2rad(20.0f) * std::sqrt(rand01(smokeSeed_));
        float b = 2.0f * PI * rand01(smokeSeed_);
        Vec3 dir{std::sin(a) * std::cos(b), std::cos(a), std::sin(a) * std::sin(b)};
        float speed = 300.0f + 60.0f * rand01(smokeSeed_);
        smoke.push_back({origin, dir * speed, 0.0f});
    }
    smokeEmit_ = std::min(smokeEmit_, 1.0f);
}

// Body, mirrored panels and tires at `base` (mPlayerCar's transform), with
// this car's part angles, steering, wheel lift and damage camber.
void Car::drawShell(Renderer &r, const Camera &cam, const Mat4 &base) const {
    const CarConfig &cfg = carConfig(carId);
    const CarMeshes &meshes = gCarMeshes[size_t(carId) - 1];
    if (meshes.body < 0) {
        // Assets unavailable: draw a small marker so the car stays visible.
        Mesh box;
        box.addBox({0, 50, 0}, {100, 40, 200}, {1, 1, 1});
        r.drawMesh(cam, base, box, &gFallbackTex, 0xffffffffu, false, false, 1.0f);
        return;
    }

    float steerRad = deg2rad(steer);

    // Director transform.rotation = (x, y, z) applies X, then Y, then Z.
    auto drawPart = [&](const Mat4 &side, int meshIndex, const Vec3 &node,
                        float rx, float ry, float rz, int cull) {
        if (meshIndex < 0) return;
        Mat4 m = side * Mat4::translation(node) * Mat4::rotationZ(rz) *
                 Mat4::rotationY(ry) * Mat4::rotationX(rx);
        drawAssetMesh(r, cam, m, gAssets.meshes[size_t(meshIndex)], false, 1.0f, true,
                      cull);
    };

    // doParts: the mirrored bonnet and boot are children of the +X halves, so
    // both halves share one angle; each door has its own. MeanieCropper's boot
    // lid drops down at three times the angle (mBoot rotation.x = -rot * 3).
    const float bonnetRx = deg2rad(partRot[0]);
    const float bootRx = deg2rad(carId == 1 ? -partRot[3] * 3.0f : partRot[3]);
    // doAnims: wheels splay out above 80 % damage (rotation.z 40 front, 20 rear).
    const bool wrecked = damage > 0.8f;
    const float camberF = wrecked ? deg2rad(40.0f) : 0.0f;
    const float camberR = wrecked ? deg2rad(20.0f) : 0.0f;
    const float spin = deg2rad(wheelSpin);

    for (int mirror = 0; mirror < 2; ++mirror) {
        Mat4 side = mirror ? base * Mat4::scale({-1.0f, 1.0f, 1.0f}) : base;
        int cull = mirror ? 2 : 1; // negative scale flips the winding
        drawPart(side, meshes.body, {0, 0, 0}, 0.0f, 0.0f, 0.0f, cull);
        drawPart(side, meshes.bonnet, cfg.bonnetPivot, bonnetRx, 0.0f, 0.0f, cull);
        drawPart(side, meshes.boot, cfg.bootPivot, bootRx, 0.0f, 0.0f, cull);
        drawPart(side, meshes.door, cfg.doorPivot, 0.0f, deg2rad(partRot[1 + mirror]), 0.0f,
                 cull);
        // doAnims: mTireFL.rotation.y = steer, mTireFR.rotation.y = -steer.
        // FR sits under the scale(-1, 1, 1) mirror, which flips the sign
        // back, so both front wheels point the same way.
        // Tire y = 16 + imbedList: the +X side uses cage 4 (front) and 2
        // (rear), the mirrored side cage 3 and 1.
        const Vec3 lift{0.0f, wheelImbed[mirror ? 2 : 3], 0.0f};
        const Vec3 liftRear{0.0f, wheelImbed[mirror ? 0 : 1], 0.0f};
        drawPart(side, meshes.tire, cfg.tirePivot + lift, spin, mirror ? -steerRad : steerRad,
                 camberF, cull);
        drawPart(side, meshes.tire, cfg.tirePivot + cfg.rearAxleOffset + liftRear, spin, 0.0f,
                 camberR, cull);
    }

}

void Car::draw(Renderer &r, const Camera &cam) const {
    if (!parked) r.drawShadow(cam, pos, 80.0f);

    const CarConfig &cfg = carConfig(carId);
    const CarMeshes &meshes = gCarMeshes[size_t(carId) - 1];
    // Surface alignment and the doBodyRoll impulses both live in rotVector, so
    // the body rotation is simply the accumulated pitch and roll.
    Mat4 base = Mat4::translation(pos) * Mat4::rotationY(yaw) *
                Mat4::rotationZ(deg2rad(bodyRollDeg)) *
                Mat4::rotationX(deg2rad(bodyPitchDeg));

    if (parked) {
        // In a Ferris wheel cage the car is drawn by the wheel; mPlayerCar is
        // the empty placeholder group.
        drawSmoke(r, cam);
        return;
    }
    drawShell(r, cam, base);
    if (meshes.body < 0) return;

    // doIgNitro: the gIgNitro group sits in the boot and turns about X by
    // igNitroRot; the bottle hangs 50 below its pivot, so a pickup flips it
    // up out of the boot and back in.
    if (gNitroBottle >= 0)
        drawAssetMesh(r, cam,
                      base * Mat4::translation(cfg.nitroGroup) *
                          Mat4::rotationX(deg2rad(igNitroRot)) *
                          Mat4::translation({0.0f, -50.0f, 0.0f}),
                      gAssets.meshes[size_t(gNitroBottle)], false, 1.0f, true);
    // Blast jets: rotation (10, 0, spin), scale.z, shader blend.
    if (gNitroBlast >= 0 && blastBlend > 0) {
        auto jet = [&](const Vec3 &at, float spin, float scaleZ) {
            Mat4 m = base * Mat4::translation(at) * Mat4::rotationZ(deg2rad(spin)) *
                     Mat4::rotationX(deg2rad(10.0f)) * Mat4::scale({1.0f, 1.0f, scaleZ});
            drawAssetMesh(r, cam, m, gAssets.meshes[size_t(gNitroBlast)], true,
                          float(blastBlend) / 100.0f, true, 0);
        };
        jet(cfg.blastL, blastSpinL, blastScaleL);
        jet(cfg.blastR, blastSpinR, blastScaleR);
    }

    drawSmoke(r, cam);
}

// Burn smoke billboards, tweened by age: size 40 -> 50, blend 100 -> 0,
// colour white -> rgb(100, 100, 0).
void Car::drawSmoke(Renderer &r, const Camera &cam) const {
    if (smoke.empty() || gBurnSmokeTex.px.empty()) return;
    Vec3 look = (cam.target - cam.pos).normalized();
    Vec3 right = look.cross(cam.up).normalized();
    Vec3 up = right.cross(look);
    for (const SmokeParticle &p : smoke) {
        float t = std::clamp(p.age, 0.0f, 1.0f);
        float h = (40.0f + 10.0f * t) * 0.5f;
        int c = int(255.0f - 155.0f * t);
        Vec3 rx = right * h, uy = up * h;
        Mesh q;
        q.addQuad(p.pos - rx - uy, p.pos + rx - uy, p.pos + rx + uy, p.pos - rx + uy, -look,
                  0.0f, 1.0f, 1.0f, 0.0f);
        r.drawMesh(cam, Mat4::identity(), q, &gBurnSmokeTex, packRGBA(c, c, int(255.0f * (1.0f - t))),
                   true, 0, 1.0f - t, false);
    }
}

} // namespace rr4
