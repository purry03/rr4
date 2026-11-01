#pragma once
// Ferris' Wheels car selector, ported from doSetupFerrisWheel,
// doResetFerrisWheel, doFerrisSwing and doFerrisWheel in
// BehaviorScript 4 - PlayerCar.ls.
//
// The wheel turns about Z; six axles hang on it 300 units out and counter-
// rotate so the cages stay level, swinging on a damped spring. Owned cars
// ride in the cages. Driving into the bottom cage parks the player's car;
// Left / Right turn the wheel 60 degrees to the next occupied cage and Down
// reverses out with the car in front of you.
#include "math.hpp"

namespace rr4 {

struct Car;
struct CarInput;
class Renderer;
struct Camera;

class FerrisWheel {
public:
    // Original position of FerrisWheel_WHEEL_001.
    static constexpr float kX = 32500.0f, kY = 440.0f, kZ = 32500.0f;

    // `owned` is g_CarsOwnedList (6 entries), shared with the race code.
    void setup(int *owned);
    // doResetFerrisWheel, also called when a race ends.
    void reset();
    // Within 4500 units this frame (the wheel is in the world).
    bool inRange() const { return visible_; }
    // doFerrisWheel: one enterFrame step. Returns true when the wheel was
    // respawned (doResetFerrisWheel), which also re-shows the first igNitro.
    bool update(Car &car, const CarInput &in, float factor);
    void draw(Renderer &r, const Camera &cam) const;

    int state() const { return state_; }

private:
    void swing(float factor);
    Mat4 wheelMatrix() const;
    Mat4 axleMatrix(int i) const;   // i = 0..5

    // g_CarsOwnedList: 2 = driven, 1 = waiting in the wheel, 0 = not owned.
    int *owned_ = nullptr;
    int cageCar_[6] = {};           // ferrisCageCarIDList, car ids 1..6
    float carDamage_[6] = {};       // carDamageList
    float carBoost_[6] = {1, 1, 1, 1, 1, 1};   // carBoostList
    float cageYaw_[6] = {};         // cage transform.rotation.y

    int state_ = 0;                 // ferrisState
    int cage_ = 1;                  // ferrisCurrentCageID (1..6)
    int enterCar_ = 0;              // enterFerrisCarID
    int drivenOutside_ = 0;         // car shell currently attached to the player
    float rot_ = 180.0f, prevRot_ = 180.0f, targetRot_ = 180.0f;
    float rotVel_ = 0.0f;
    float calcAngle_ = 0.0f;
    float swing_ = 0.0f, swingVel_ = 0.0f;
    bool spawned_ = false, visible_ = false;
};

} // namespace rr4
