#include "cinematic.hpp"
#include "world.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>

namespace rr4 {

namespace {

constexpr int kFadeTicks = 40;           // fade out and in between shots
constexpr int kPlaneLeadTicks = 380;     // plane approach run before its shot
constexpr float kMenuFov = 50.0f;

enum class Move { Dolly, Orbit, TrackPlane };

// Dolly: camera from -> to, looking from lookFrom -> lookTo.
// Orbit: around `centre` at `radius`, from angle `fromDeg` by `sweepDeg`,
//        camera height from.y -> to.y, looking at lookFrom.
// TrackPlane: camera fixed at `from`, looking at the plane.
struct Shot {
    Move move;
    int ticks;
    Vec3 from, to, lookFrom, lookTo;
    Vec3 centre;
    float radius, fromDeg, sweepDeg;
    float fov;
};

const Shot kShots[] = {
    // Ferris' Wheels, slow orbit.
    {Move::Orbit, 600, {0, 520, 0}, {0, 760, 0}, {32500, 700, 32500}, {}, {32500, 0, 32500},
     2600.0f, 200.0f, 50.0f, kMenuFov},
    // Low along the long north-south street, traffic passing.
    {Move::Dolly, 540, {14150, 70, 33600}, {14150, 95, 37600}, {14550, 60, 35600}, {14550, 60, 39600},
     {}, 0, 0, 0, 45.0f},
    // Past the Golden Gate Bridge.
    {Move::Dolly, 600, {54000, 900, 25000}, {54500, 700, 34000}, {57500, 450, 28000},
     {57500, 450, 37000}, {}, 0, 0, 0, kMenuFov},
    // The 747 landing, from beside the runway.
    {Move::TrackPlane, 720, {10200, 140, 30000}, {}, {}, {}, {}, 0, 0, 0, 40.0f},
    // High over the city towards the Ferris wheel.
    {Move::Dolly, 600, {24000, 2200, 22000}, {31000, 1800, 29000}, {34000, 300, 33000},
     {34000, 300, 33000}, {}, 0, 0, 0, kMenuFov},
    // Around the Nurd Burger ring.
    {Move::Orbit, 600, {0, 2600, 0}, {0, 2300, 0}, {32500, 400, 47500}, {}, {32500, 0, 47500},
     3200.0f, 150.0f, 40.0f, kMenuFov},
};
constexpr int kShotCount = int(std::size(kShots));

Vec3 mix(const Vec3 &a, const Vec3 &b, float t) { return a + (b - a) * t; }
float smooth(float t) { return t * t * (3.0f - 2.0f * t); }

} // namespace

void CinematicCamera::startShot(World &world) {
    shot_ = (shot_ + 1) % kShotCount;
    tick_ = 0;
    if (kShots[shot_].move == Move::TrackPlane) {
        // Put the plane on its approach and let it come in close.
        world.plane.startApproach();
        for (int i = 0; i < kPlaneLeadTicks; ++i) world.plane.update(world.ticks);
    }
}

void CinematicCamera::update(World &world) {
    if (shot_ < 0 || tick_ >= kShots[shot_].ticks) startShot(world);
    const Shot &s = kShots[shot_];
    const float t = smooth(float(tick_) / float(s.ticks));

    switch (s.move) {
    case Move::Dolly:
        cam_.pos = mix(s.from, s.to, t);
        cam_.target = mix(s.lookFrom, s.lookTo, t);
        break;
    case Move::Orbit: {
        const float a = deg2rad(s.fromDeg + s.sweepDeg * t);
        cam_.pos = s.centre + Vec3(std::sin(a) * s.radius, 0, std::cos(a) * s.radius);
        cam_.pos.y = s.from.y + (s.to.y - s.from.y) * t;
        cam_.target = s.lookFrom;
        break;
    }
    case Move::TrackPlane:
        cam_.pos = s.from;
        // Ease towards the plane so the camera pans like a camera operator.
        cam_.target = tick_ == 0 ? world.plane.position()
                                 : mix(cam_.target, world.plane.position(), 0.08f);
        break;
    }
    cam_.up = {0, 1, 0};
    cam_.fovDeg = s.fov;

    const int left = s.ticks - tick_;
    fade_ = std::clamp(1.0f - float(std::min(tick_, left)) / float(kFadeTicks), 0.0f, 1.0f);

    world.updateAmbient(cam_.target);
    ++tick_;
}

} // namespace rr4
