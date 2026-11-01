#pragma once
#include "math.hpp"
#include "renderer.hpp"

#include <functional>

namespace rr4 {

struct CarInput {
    bool up = false, down = false, left = false, right = false, boost = false;
};

// Per-car data from BehaviorScript 4 - PlayerCar.ls (doSetupShells,
// doControls) and the original 3DOM car part meshes. Kept as a table so the
// rendered shell, physics, paint tint and engine Sfx can be switched together.
struct CarConfig {
    const char *name;
    const char *body;
    const char *bonnet;
    const char *boot;
    const char *door;
    const char *tire;
    Vec3 bonnetPivot;
    Vec3 bootPivot;
    Vec3 doorPivot;
    Vec3 tirePivot;
    Vec3 rearAxleOffset;   // doSetupShells tempTireBL.translate(...)
    float accel;           // doControls case playerCarID
    uint32_t shellTint;    // runtime paint (original paint casts are absent)
    const char *shellMaterial;
    const char *engineUp;
    const char *engineDown;
    int engineUpMember;    // Sfx slot = playerCarID * 2 - 1
    int engineDownMember;  // Sfx slot = playerCarID * 2
};

int carCount();
const CarConfig &carConfig(int id); // id 1..6

// Physics constants are taken verbatim from the original PlayerCar behavior
// (BehaviorScript 4 - PlayerCar.ls). Default car is #1 (MeanieCropper); use
// Car::setCar() to switch to any of the six original shells.
struct Car {
    int carId = 1;
    Vec3 pos{32053.57f, -5.56f, 30797.51f};
    float yaw = deg2rad(45.0f);       // mPlayerCar.transform.rotation.y
    Vec3 vel;                          // velVector
    float steer = 0.0f;                // steer, -40..40
    float throttle = 0.0f;             // throttle, -8..10
    float yawVelDeg = 0.0f;            // rotVector.y (degrees / frame)
    float rotXDeg = 0.0f;              // rotVector.x (pitch, degrees)
    float rotZDeg = 0.0f;              // rotVector.z (roll, degrees)
    float boostMeter = 1.0f;           // g_BoostPercent
    float damage = 0.0f;               // g_DamagePercent
    bool midAir = false;
    float wheelSpin = 0.0f;            // accumulated tire rotation, degrees
    float surfacePitchDeg = 0.0f;      // doRayCasting slope response, degrees
    float surfaceRollDeg = 0.0f;       // doRayCasting slope response, degrees
    float speedKmh = 0.0f;
    float landImpact = 0.0f;           // downward speed at touchdown (m/s units)
    float impactSpeed = 0.0f;          // wall impulse magnitude, for CrashTinkle

    // Ground height sampled from the original tile meshes by World.
    float groundY = 0.0f;
    bool useGroundMap = false;

    // mechanics
    // `collide` is the original doCollisions pass. It runs inside the step,
    // after doTireGrip and before doBodyRoll, because the original enterFrame
    // order is ... doTireGrip -> doCollisions -> doBodyRoll -> doupdate.
    void update(const CarInput &in, float factor,
                const std::function<void(Car &)> &collide = {});
    void setCar(int id);
    Vec3 forward() const { return Vec3(std::sin(yaw), 0, std::cos(yaw)); }
    Vec3 rightAxis() const { return Vec3(std::cos(yaw), 0, -std::sin(yaw)); }
    float forwardSpeed() const { return vel.dot(forward()); }
    float lateralSpeed() const { return vel.dot(rightAxis()); }

    // rendering
    void draw(Renderer &r, const Camera &cam) const;

private:
    Vec3 prevVel_;
};

// Loads the original HighWire car parts from the Shockwave 3D asset pack.
void buildCarAssets();

} // namespace rr4
