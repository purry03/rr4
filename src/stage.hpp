#pragma once
// Point stages (time attacks): doSetupPointsFlag / doPointsFlag and
// doSetupPointPods / doPointPods (BehaviorScript 4 - PlayerCar.ls).
//
// The points flag stands at whichever of the eight stage starts is nearest.
// Lining up on it runs Ready / Set / Go, then the countdown starts and every
// green pod reached is worth 100 points; the time left at the last pod is
// added as a bonus. Time running out fails the stage.

#include "math.hpp"
#include "renderer.hpp"

namespace rr4 {

struct Car;
struct World;

// tools/gen_stage_data.py -> src/stage_data.gen.cpp
struct StageDef {
    Vec3 flag;          // pointsFlagPosList
    float flagYawDeg;   // pointsFlagRotList (y)
    const Vec3 *pods;   // stagePodPosList
    int podCount;
};
extern const StageDef kStages[8];

class Stage {
public:
    void setup();
    // doPointsFlag then doPointPods.
    void update(Car &player, World &world);
    void draw(Renderer &r, const Camera &cam) const;

    int state() const { return state_; }          // stageState
    bool flagShown() const { return state_ == 0 && ticks_ >= flagReadyTicks_; }
    Vec3 flagPosition() const { return kStages[current_ - 1].flag; }
    bool podInWorld() const { return podVisible_; }   // mPointPod in the world
    Vec3 podPosition() const { return podAt_; }

private:
    void placePods();
    void hidePods(World &world);

    int state_ = 0;
    int current_ = 1;           // currentStage
    int podPointer_ = 1;        // pointPodPointer
    int lightPhase_ = 0;        // raceLightPhase
    long ticks_ = 0, startTicks_ = 0, lightStartTicks_ = 0;
    int prevState_ = 0;
    long flagReadyTicks_ = 0;   // the flag comes back 5 s after a stage
    float lightsU_ = 0.0f, arrowsV_ = 0.0f, mistU_ = 0.0f, mistV_ = 0.0f;
    bool podVisible_ = false, nextPodVisible_ = false;
    Vec3 podAt_, nextPodAt_;
    float podYaw_ = 0.0f, nextPodYaw_ = 0.0f;
};

} // namespace rr4
