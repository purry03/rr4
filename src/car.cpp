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

// Data from doSetupShells and doControls. Pivots are reconstructed from the
// original +X-half part geometry (hinge at the panel edge nearest the body,
// tire inner edge at the body's outer skin) because the car node transforms are
// not present in the 3DOM stream. Rear axle offsets are the literal
// tempTireBL.translate values from doSetupShells.
const CarConfig kCars[6] = {
    {"MeanieCropper", "MeanieCropper_BODY_000", "MeanieCropper_BONNET_000",
     "MeanieCropper_BOOT_000", "MeanieCropper_DOOR_000", "MeanieCropper_TIRE_000",
     {0.0f, 65.2f, 55.3f}, {0.0f, 55.0f, -60.0f}, {40.6f, 0.0f, 17.0f},
     {38.3f, 16.0f, 68.4f}, {0.0f, 0.0f, -130.0f}, 0.0250f,
     packRGBA(226, 88, 44), "Texture_MeanieShellb",
     "Meanie_RevvingUp_001", "Meanie_RevvingDown_000", 1, 2},
    {"Bitsasushi", "Bitsasushi_BODY_000", "Bitsasushi_BONNET_000",
     "Bitsasushi_BOOT_000", "Bitsasushi_DOOR_000", "Bitsasushi_TIRE_000",
     {0.0f, 62.7f, 73.3f}, {0.0f, 61.4f, -84.4f}, {42.0f, 0.0f, 17.1f},
     {39.2f, 16.0f, 87.7f}, {2.0f, 0.0f, -150.0f}, 0.0325f,
     packRGBA(200, 52, 40), "texture_BitsasushiShellT",
     "Bitsasushi_RevvingUp_000", "Bitsasushi_RevvingDown_001", 3, 4},
    {"Amazed", "Amazed_BODY_012", "Amazed_BONNET_000",
     "Amazed_BOOT_000", "Amazed_DOOR_000", "Amazed_TIRE_000",
     {0.0f, 57.3f, 58.4f}, {0.0f, 56.1f, -97.3f}, {49.4f, 0.0f, 16.7f},
     {38.4f, 16.0f, 88.6f}, {2.0f, 0.0f, -160.0f}, 0.0350f,
     packRGBA(62, 122, 216), "Texture_AmazedShell",
     "Amazed_RevvingUp_000", "Amazed_RevvingDown_003", 5, 6},
    {"HighWire", "HighWire_BODY_000", "HighWire_BONNET_000",
     "HighWire_BOOT_000", "HighWire_DOOR_000", "HighWire_TIRE_000",
     {0.0f, 58.2f, 61.1f}, {0.0f, 57.0f, -71.4f}, {47.8f, 0.0f, 20.4f},
     {42.1f, 16.0f, 89.7f}, {2.0f, 0.0f, -134.0f}, 0.0375f,
     packRGBA(226, 88, 44), "Texture_HighWireShell",
     "skyline_RevvingUp_000", "skyline_RevvingDown_000", 7, 8},
    {"Posher", "Posher_BODY_007", "Posher_BONNET_000",
     "Posher_BOOT_000", "Posher_DOOR_000", "Posher_TIRE_000",
     {0.0f, 59.5f, 54.3f}, {0.0f, 58.2f, -97.7f}, {52.5f, 0.0f, 9.5f},
     {42.8f, 16.0f, 86.2f}, {2.0f, 0.0f, -150.0f}, 0.0400f,
     packRGBA(72, 180, 96), "texture_PosherShell",
     "Posher_RevvingUp_000", "Posher_RevvingDown_001", 9, 10},
    {"Furore", "Furore_BODY_000", "FuroreI_BONNET_000",
     "FuroreI_BOOT_000", "FuroreI_DOOR_000", "FuroreI_TIRE_000",
     {0.0f, 54.7f, 63.4f}, {0.0f, 53.5f, -57.2f}, {44.2f, 0.0f, 19.3f},
     {43.2f, 16.0f, 88.3f}, {2.0f, 0.0f, -150.0f}, 0.0425f,
     packRGBA(240, 200, 62), "Texture_FuroreShellW",
     "Furore_RevvingUp_000", "Furore_RevvingDown_002", 11, 12},
};

Texture gFallbackTex;

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

        // The original game repaints the shell at runtime by swapping a
        // shader texture for one of the paint-colour cast members. Those cast
        // members are not in the 3DOM stream, so tint the original shell
        // texture with the car's livery colour.
        for (AssetMaterial &mat : gAssets.materials)
            if (mat.name == cfg.shellMaterial)
                mat.color = cfg.shellTint;
    }

    // Fallback only used when the original textures are absent from a pack.
    gFallbackTex.w = gFallbackTex.h = 2;
    gFallbackTex.px = {packRGBA(214, 92, 40), packRGBA(214, 92, 40),
                       packRGBA(214, 92, 40), packRGBA(214, 92, 40)};
}

void Car::update(const CarInput &in, float factor) {
    const bool wasAir = midAir;
    landImpact = 0.0f;
    impactSpeed = 0.0f;

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
    if (in.up && !in.down) {
        if (throttle < 0) throttle = 0;
        throttle += 0.2f * factor;
    }
    if (in.down && !in.up) {
        if (throttle > 0) throttle = 0;
        throttle -= 0.2f * factor;
    }
    if (!in.up && !in.down) throttle = 0;
    throttle = clampf(throttle, -8.0f, 10.0f);

    Vec3 fwd = forward();
    Vec3 right = rightAxis();

    // doControls case playerCarID: car #1 = 0.025, #6 = 0.0425.
    if (!midAir) {
        vel += fwd * (throttle * carConfig(carId).accel * factor);
        if (in.boost && boostMeter > 0.0f) {
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

    // ---- doBodyRoll -------------------------------------------------------
    // diffVector = prevVelVector - velVector, horizontal, capped at 1.
    Vec3 diff = prevVel_ - vel;
    diff.y = 0.0f;
    if (diff.length() > 1.0f) diff = diff.normalized();
    rotXDeg -= diff.z * 0.35f * factor;
    rotZDeg += diff.x * 0.35f * factor;
    prevVel_ = vel;

    // ---- doupdate ---------------------------------------------------------
    pos += vel * factor;
    yaw += deg2rad(yawVelDeg * factor);
    if (yaw > PI) yaw -= 2 * PI;
    if (yaw < -PI) yaw += 2 * PI;

    // Ground height: flat for standalone physics traces, sampled from the
    // original tile meshes when World is driving the car.
    float gy = useGroundMap ? groundY : 0.0f;
    if (pos.y <= gy) {
        pos.y = gy;
        if (wasAir && vel.y < 0.0f) landImpact = -vel.y;
        if (vel.y < 0) vel.y = 0;
        midAir = false;
    } else {
        midAir = true;
    }

    // Map bounds and building collisions are resolved by World after the step.

    // ---- doAnims (tire rotation) -----------------------------------------
    // rollVel = normalized(vel*1.6) . zAxis * |vel*1.6|, then the original
    // adds throttle * 2 before rotating the tires.
    Vec3 rollVec = vel * 1.6f;
    rollVec.y = 0.0f;
    float rollFactor = rollVec.length();
    float rollVel = 0.0f;
    if (rollFactor > 1e-4f) rollVel = (rollVec / rollFactor).dot(fwd) * rollFactor;
    wheelSpin += (rollVel + throttle * 2.0f) * factor;

    speedKmh = std::fabs(fwdSpeed) * 60.0f * 0.036f;
}

void Car::draw(Renderer &r, const Camera &cam) const {
    r.drawShadow(cam, pos, 80.0f);

    const CarConfig &cfg = carConfig(carId);
    const CarMeshes &meshes = gCarMeshes[size_t(carId) - 1];
    // Surface alignment from the tile height field plus doBodyRoll.
    Mat4 base = Mat4::translation(pos) * Mat4::rotationY(yaw) *
                Mat4::rotationZ(deg2rad(rotZDeg + surfaceRollDeg)) *
                Mat4::rotationX(deg2rad(rotXDeg + surfacePitchDeg));

    if (meshes.body < 0) {
        // Assets unavailable: draw a small marker so the car stays visible.
        Mesh box;
        box.addBox({0, 50, 0}, {100, 40, 200}, {1, 1, 1});
        r.drawMesh(cam, base, box, &gFallbackTex, 0xffffffffu, false, false, 1.0f);
        return;
    }

    float steerRad = deg2rad(steer);

    auto drawPart = [&](const Mat4 &side, int meshIndex, const Vec3 &node,
                        float steerAngle, float spin, int cull) {
        if (meshIndex < 0) return;
        Mat4 m = side * Mat4::translation(node) *
                 Mat4::rotationY(steerAngle) * Mat4::rotationX(spin);
        drawAssetMesh(r, cam, m, gAssets.meshes[size_t(meshIndex)], false, 1.0f, true,
                      cull);
    };

    for (int mirror = 0; mirror < 2; ++mirror) {
        Mat4 side = mirror ? base * Mat4::scale({-1.0f, 1.0f, 1.0f}) : base;
        int cull = mirror ? 2 : 1; // negative scale flips the winding
        drawPart(side, meshes.body, {0, 0, 0}, 0.0f, 0.0f, cull);
        drawPart(side, meshes.bonnet, cfg.bonnetPivot, 0.0f, 0.0f, cull);
        drawPart(side, meshes.boot, cfg.bootPivot, 0.0f, 0.0f, cull);
        drawPart(side, meshes.door, cfg.doorPivot, 0.0f, 0.0f, cull);
        // Front tires steer (rotation.y = steer / -steer in the original);
        // the reflection of the mirror side supplies the opposite sign.
        drawPart(side, meshes.tire, cfg.tirePivot, steerRad, deg2rad(wheelSpin), cull);
        drawPart(side, meshes.tire, cfg.tirePivot + cfg.rearAxleOffset, 0.0f,
                 deg2rad(wheelSpin), cull);
    }
}

} // namespace rr4
