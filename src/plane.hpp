#pragma once
// The airport Boeing 747: doSetupPlane / doPlane (BehaviorScript 4 -
// PlayerCar.ls). A sixteen-state loop: take off along the runway, climb and
// bank away east, reappear 70000 north, land, taxi to the stand, wait, back
// out and turn round for the next take-off.

#include "math.hpp"
#include "renderer.hpp"

namespace rr4 {

class Plane {
public:
    void setup();
    void update(long ticks);   // doPlane, once per frame
    // Stage 8 start: put the plane on its approach (planeState 8).
    void startApproach();
    Vec3 position() const { return pos_; }
    // mPlane's world transform (gPlane position and Y turn, then the model's
    // own tilt and bank); gPlaneCollider is its child.
    Mat4 transform() const;
    void draw(Renderer &r, const Camera &cam) const;

private:
    // gPlane.translate(x, y, z, #self)
    void translate(float x, float y, float z);

    int state_ = 7;             // planeState
    Vec3 pos_;                  // gPlane.worldPosition
    float groupYaw_ = 0.0f;     // gPlane.transform.rotation.y, degrees
    float yRot_ = 0.0f;         // planeYRot
    float speed_ = 0.0f;        // planeSpeed
    float tilt_ = 0.0f;         // planeTilt
    float bank_ = 0.0f;         // planeBank
    long startTicks_ = 0;       // planeStartTicks
    int mesh_ = -1;
};

} // namespace rr4
