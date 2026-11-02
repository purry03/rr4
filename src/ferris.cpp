#include "ferris.hpp"
#include "garage.hpp"
#include "assets.hpp"
#include "car.hpp"

#include <cmath>

namespace rr4 {

namespace {
const Vec3 kWheelPos{FerrisWheel::kX, FerrisWheel::kY, FerrisWheel::kZ};

int gWheelMesh = -1, gAxleMesh = -1, gCageMesh = -1;

// Axle i (1-based) sits at angle 30 + 60 i on a 300 unit radius.
Vec3 axleOffset(int i) {
    float a = deg2rad(30.0f + 60.0f * float(i + 1));
    return {300.0f * std::cos(a), 300.0f * std::sin(a), 0.0f};
}

// The player's car may enter the bottom cage from this box around the wheel.
bool inEntryZone(const Vec3 &diff) {
    return diff.z < 200.0f && diff.z > -360.0f && std::fabs(diff.x) < 100.0f;
}
} // namespace

bool FerrisWheel::cullTo(const Vec3 &focus) {
    Vec3 diff = kWheelPos - focus;
    diff.y = 0.0f;
    if (diff.length() >= kSpawnDist) {
        spawned_ = visible_ = false;
        return false;
    }
    visible_ = true;
    if (spawned_) return false;
    reset();
    spawned_ = true;
    return true;
}

void FerrisWheel::setup(int *owned) {
    gWheelMesh = gAssets.meshIndex("FerrisWheel_WHEEL_001");
    gAxleMesh = gAssets.meshIndex("FerrisWheel_AXLE_000");
    gCageMesh = gAssets.meshIndex("FerrisWheel_CAGE_001");
    owned_ = owned;
    reset();
    spawned_ = false;
}

// doResetFerrisWheel: empty the bottom cage, fill cages 2.. with the waiting
// cars in id order and put the wheel back at 180 degrees.
void FerrisWheel::reset() {
    state_ = 0;
    calcAngle_ = 0.0f;
    cage_ = 1;
    for (int &c : cageCar_) c = 0;
    rot_ = targetRot_ = 180.0f;
    rotVel_ = 0.0f;
    swing_ = swingVel_ = 0.0f;
    int next = 2;
    for (int i = 0; i < 6; ++i)
        if (owned_[i] == kOwnedParked && next <= kCarCount) cageCar_[(next++) - 1] = i + 1;
    swing(1.0f);
}

Mat4 FerrisWheel::wheelMatrix() const {
    return Mat4::translation(kWheelPos) * Mat4::rotationZ(deg2rad(rot_));
}

Mat4 FerrisWheel::axleMatrix(int i) const {
    return wheelMatrix() * Mat4::translation(axleOffset(i)) *
           Mat4::rotationZ(deg2rad(-rot_ + swing_));
}

// doFerrisSwing: damp the swing and turn velocity, spring the swing back,
// keep the axles level against the wheel and turn the bottom cage(s) so the
// car faces out of the wheel as it passes the loading point.
void FerrisWheel::swing(float factor) {
    swingVel_ -= swingVel_ * 0.02f * factor;
    rotVel_ -= rotVel_ * 0.04f * factor;
    swingVel_ -= swing_ * 0.005f * factor;
    swing_ += swingVel_;
    for (int i = 0; i < 6; ++i) {
        Vec3 axle = wheelMatrix().transformPoint(axleOffset(i));
        if (axle.y < 324.0f) {
            float x = clampf(kWheelPos.x + 165.0f - axle.x, 0.0f, 330.0f) / 330.0f;
            cageYaw_[i] = x * 360.0f - 180.0f;
        } else {
            cageYaw_[i] = 180.0f;
        }
    }
}

bool FerrisWheel::update(Car &car, const CarInput &in, float factor) {
    bool respawned = false;
    Vec3 diff = kWheelPos - car.pos;
    diff.y = 0.0f;

    auto pin = [&] {
        car.vel = {};
        car.yawVelDeg = car.rotXDeg = car.rotZDeg = 0.0f;
        car.pos.x = kWheelPos.x;
        car.pos.z = kWheelPos.z;
    };
    auto select = [&](int step, float target) {
        int next = cage_ + step;
        if (next > 6) next = 1;
        if (next < 1) next = 6;
        if (cageCar_[next - 1] <= 0) return;
        cage_ = next;
        int id = cageCar_[cage_ - 1];
        car.damage = carDamage_[id - 1];
        car.boostMeter = carBoost_[id - 1];
        state_ = 3;
        prevRot_ = rot_;
        targetRot_ = target;
        calcAngle_ = 0.0f;
        car.ferrisTurnSfx = true;
    };

    switch (state_) {
    case 0:
        if (inEntryZone(diff) && car.damage < 1.0f) state_ = 1;
        if (cullTo(car.pos)) respawned = true;
        break;
    case 1:
        if (inEntryZone(diff)) {
            car.pos.x = kWheelPos.x;
            car.vel.x = 0.0f;
            car.yawVelDeg = car.rotXDeg = car.rotZDeg = 0.0f;
            car.yaw *= 0.95f;   // mPlayerCar.transform.rotation * 0.95
            car.bodyPitchDeg *= 0.95f;
            car.bodyRollDeg *= 0.95f;
            if (car.pos.z > kWheelPos.z) {
                // Park the car in the bottom cage.
                carDamage_[car.carId - 1] = car.damage;
                carBoost_[car.carId - 1] = car.boostMeter;
                car.pos.z = kWheelPos.z;
                state_ = 2;
                car.ferrisCreakSfx = true;
                enterCar_ = car.carId;
                cageCar_[cage_ - 1] = car.carId;
                car.parked = true;
                car.doPanel(8, 0);       // FERRIS' WHEELS instructions
                car.saveGame = true;
                car.yaw = car.bodyPitchDeg = car.bodyRollDeg = 0.0f;
                car.resetParts();
            }
        }
        swing(factor);
        break;
    case 2:
        pin();
        if (in.down && !in.up) {
            // Reverse out with the car in front of you.
            state_ = 0;
            owned_[enterCar_ - 1] = kOwnedParked;
            int id = cageCar_[cage_ - 1];
            owned_[id - 1] = kOwnedDriving;
            car.setCar(id);
            car.saveGame = true;
            car.parked = false;
            car.yaw = car.bodyPitchDeg = car.bodyRollDeg = 0.0f;
            car.doPanel(0, 0);
        } else {
            // Any Left / Right press hides the panel, even with no car to
            // turn to.
            if (in.right) { select(+1, 60.0f); car.doPanel(0, 0); }
            if (in.left) { select(-1, -60.0f); car.doPanel(0, 0); }
        }
        swing(factor);
        break;
    case 3: {
        pin();
        calcAngle_ += 1.4f * factor;
        if (calcAngle_ >= 180.0f) {
            calcAngle_ = 180.0f;
            state_ = 2;
            car.doPanel(cageCar_[cage_ - 1], 0);   // infoPanels 1..6: the car
        }
        float before = rot_;
        rot_ = prevRot_ + targetRot_ * ((std::cos(deg2rad(calcAngle_)) - 1.0f) / 2.0f);
        swingVel_ += (before - rot_) * 0.2f;
        swing(factor);
        break;
    }
    }
    drivenOutside_ = state_ < 2 ? car.carId : 0;
    return respawned;
}

void FerrisWheel::draw(Renderer &r, const Camera &cam) const {
    if (!visible_ || gWheelMesh < 0) return;
    drawAssetMesh(r, cam, wheelMatrix(), gAssets.meshes[size_t(gWheelMesh)], false, 1.0f, true);
    for (int i = 0; i < 6; ++i) {
        Mat4 axle = axleMatrix(i);
        if (gAxleMesh >= 0)
            drawAssetMesh(r, cam, axle, gAssets.meshes[size_t(gAxleMesh)], false, 1.0f, true);
        Mat4 cage = axle * Mat4::rotationY(deg2rad(cageYaw_[i]));
        if (gCageMesh >= 0)
            drawAssetMesh(r, cam, cage, gAssets.meshes[size_t(gCageMesh)], false, 1.0f, true);
        int id = cageCar_[i];
        if (id <= 0 || id == drivenOutside_) continue;
        Car shell;
        shell.setCar(id);
        shell.damage = carDamage_[id - 1];
        shell.drawShell(r, cam, cage * Mat4::translation({0.0f, -135.0f, 0.0f}));
    }
}

} // namespace rr4
