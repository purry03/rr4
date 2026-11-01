#pragma once
#include "math.hpp"
#include "renderer.hpp"

#include <functional>
#include <vector>

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
    // Part node positions relative to the BODY node, read from the original
    // 3DEM node blocks. Each part hinges about its node origin.
    Vec3 bonnetPivot;
    Vec3 bootPivot;
    Vec3 doorPivot;
    Vec3 tirePivot;        // x/z from the node; y = 16 (doAnims adds imbedList)
    Vec3 rearAxleOffset;   // doSetupShells tempTireBL.translate(...)
    float accel;           // doControls case playerCarID
    // doSetupShells / doResetFerrisWheel: mShellList[car].shaderList[paintShader]
    // .texture.member = member(150 + g_CarsColorIDList[car], "Textures").
    int paintShader;       // 1-based shader (BODY mesh group) index
    int paintColor;        // g_CarsColorIDList default [1, 2, 3, 4, 5, 1]
    const char *engineUp;
    const char *engineDown;
    int engineUpMember;    // Sfx slot = playerCarID * 2 - 1
    int engineDownMember;  // Sfx slot = playerCarID * 2
    // doSetupPlayerCar: gIgNitro group (boot bottle pivot) and the two
    // igNitroBlastJet exhaust positions, relative to mPlayerCar.
    Vec3 nitroGroup;
    Vec3 blastL, blastR;
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
    float rotXDeg = 0.0f;              // rotVector.x: pitch angular velocity, deg/frame
    float rotZDeg = 0.0f;              // rotVector.z: roll angular velocity, deg/frame
    float bodyPitchDeg = 0.0f;         // mPlayerCar.transform.rotation.x, deg
    float bodyRollDeg = 0.0f;          // mPlayerCar.transform.rotation.z, deg
    float hoist = 0.0f;                // doRayCasting hoist, applied by doupdate
    float wheelImbed[4] = {};          // imbedList[1..4]: wheel cage ray depth
    float boostMeter = 1.0f;           // g_BoostPercent
    float damage = 0.0f;               // g_DamagePercent
    bool midAir = false;
    float wheelSpin = 0.0f;            // accumulated tire rotation, degrees
    float groundPitchDeg = 0.0f;       // measured ground slope under the cage
    float groundRollDeg = 0.0f;        // measured ground slope under the cage
    float speedKmh = 0.0f;
    float landImpact = 0.0f;           // downward speed at touchdown (m/s units)
    float impactSpeed = 0.0f;          // wall impulse magnitude, for CrashTinkle
    bool respawned = false;            // doDamageMaxOutReset ran this step
    bool raceReturn = false;           // race end moved the car to the Ferris spawn

    // Requests to the camera-overlay behaviours made by gameplay code this
    // step, applied in order by tickHud (InfoPanels doPanel, FadeToBlack,
    // the Score&Timer g_TimerState).
    struct PanelRequest { int id; int ticks; };
    std::vector<PanelRequest> panels;
    bool fadeToBlack = false;          // sprite(1).fadeToBlackBlend = 100
    std::vector<int> sfx;              // puppetSound(1, member(n, "Sfx")) requests
    int timerState = kTimerUnchanged;  // new g_TimerState
    bool timerReset = false;           // timeList = [0, 0, 0, 0, 0]
    static constexpr int kTimerUnchanged = -100;
    void doPanel(int id, int ticks) { panels.push_back({id, ticks}); }

    // PlayerCar race state the car's own handlers depend on.
    bool inRace = false;               // raceState > 0
    bool raceWrecked = false;          // damage maxed out during a race
    float igNitroRot = 0.0f;           // doIgNitro: boot bottle flip, degrees
    bool nitroPickedUp = false;        // doIgNitro collected a bottle this step
    // doAnims igNitroBlastJet state: shader blend 0..100, scale.z, spin.
    int blastBlend = 0;
    float blastScaleL = 0.2f, blastScaleR = 0.2f;
    float blastSpinL = 0.0f, blastSpinR = 0.0f;
    bool parked = false;               // in a Ferris wheel cage (ferrisState >= 2)
    bool ferrisCreakSfx = false;       // doFerrisWheel: Sfx 21 when parking
    bool ferrisTurnSfx = false;        // doFerrisWheel: Sfx 22 when turning

    int repairState = 0;               // doRixRapidRepairs: 1 while inside Rix's
    bool repairSfx = false;            // doRixRapidRepairs wants Sfx member 24

    // doParts: bonnet, left door, right door, boot (partRotList 1..4, degrees).
    float partRot[4] = {};
    float partRotVel[4] = {};

    // doSetupBurnSmoke particle, emitted while damage >= 0.6.
    struct SmokeParticle {
        Vec3 pos, vel;       // world units, units / second
        float age = 0.0f;    // seconds
    };
    std::vector<SmokeParticle> smoke;

    // Ground height sampled from the original tile meshes by World.
    float groundY = 0.0f;
    bool useGroundMap = false;

    // mechanics
    // `ground` is the original doRayCasting pass and runs first in the step;
    // `collide` is doCollisions and runs between doTireGrip and doBodyRoll.
    // Original enterFrame order: doRayCasting -> doControls -> doGravity ->
    // doFriction -> doTireGrip -> doCollisions -> doBodyRoll -> doupdate.
    void update(const CarInput &in, float factor,
                const std::function<void(Car &)> &ground = {},
                const std::function<void(Car &)> &collide = {});
    void setCar(int id);
    Vec3 forward() const { return Vec3(std::sin(yaw), 0, std::cos(yaw)); }
    Vec3 rightAxis() const { return Vec3(std::cos(yaw), 0, -std::sin(yaw)); }
    float forwardSpeed() const { return vel.dot(forward()); }
    float lateralSpeed() const { return vel.dot(rightAxis()); }

    // rendering
    void draw(Renderer &r, const Camera &cam) const;
    void drawShell(Renderer &r, const Camera &cam, const Mat4 &base) const;

    void resetParts() { for (int i = 0; i < 4; ++i) partRot[i] = partRotVel[i] = 0.0f; }

private:
    void doParts(const CarInput &in, float factor);
    void doBurnSmoke(float factor);
    void doNitroBlast(const CarInput &in, float factor);
    void drawSmoke(Renderer &r, const Camera &cam) const;
    void damageMaxOutReset();

    Vec3 prevVel_;
    Vec3 partsPrevVel_;            // prevVelVector at the end of the last step
    float prevDamage_ = 0.0f;      // prevDamagePercent
    float wreckTicks_ = -1.0f;     // ticks since damage maxed out, -1 = idle
    float smokeEmit_ = 0.0f;       // fractional particles owed by the emitter
    uint32_t smokeSeed_ = 0x2545f491u;
};

// Loads the original car parts from the Shockwave 3D asset pack and the
// burn smoke particle bitmap.
void buildCarAssets();

} // namespace rr4
