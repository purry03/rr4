#pragma once
// Main menu background: a real-time tour of IgNitro City. A fixed list of
// camera shots (dollies, orbits and a plane-tracking shot) plays in a loop
// over the live world, with a fade through black between shots.

#include "renderer.hpp"

namespace rr4 {

struct World;

class CinematicCamera {
public:
    // Advance one 60 Hz tick: moves the camera and runs the ambient world
    // (traffic, lights, plane) around what the camera is looking at.
    void update(World &world);
    const Camera &camera() const { return cam_; }
    // 0 = clear, 1 = black (between shots).
    float fade() const { return fade_; }

private:
    void startShot(World &world);

    int shot_ = -1;
    int tick_ = 0;
    float fade_ = 1.0f;
    Camera cam_;
};

} // namespace rr4
