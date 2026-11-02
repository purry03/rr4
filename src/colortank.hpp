#pragma once
#include "garage.hpp"
// The Splash of Color machine next to Ferris' Wheels: doSetupColorTank /
// doColorTank (BehaviorScript 4 - PlayerCar.ls). Drive under the paint cans
// to park; LEFT / RIGHT turn the cans to the previous / next paint job and
// pour it over the car, REVERSE leaves.

#include "math.hpp"
#include "renderer.hpp"

namespace rr4 {

struct Car;
struct CarInput;

class ColorTank {
public:
    void setup();
    // carColors: g_CarsColorIDList, updated when the paint lands.
    void update(Car &car, const CarInput &in, int carColors[kCarCount]);
    void draw(Renderer &r, const Camera &cam) const;

private:
    void lockCar(Car &car) const;
    void pour(int newID, int prevID, bool right);

    // colorTankState
    enum State { kIdle = 0, kParked = 1, kSwing = 2, kPour = 3 };
    State state_ = kIdle;
    int splashID_ = 1;          // colorSplashID
    float splash_ = 0.0f;       // colorSplashPercent
    float rot_ = 0.0f, prevRot_ = 0.0f, targetRot_ = 0.0f, angle_ = 0.0f;
    // shaderList[1] / [2] can textures (ColorCanN) and their V positions.
    int can1_ = 1, can2_ = 1;
    float can1V_ = 0.0f, can2V_ = 0.0f;
};

} // namespace rr4
