#pragma once
// Races, ported from doSetupRaceFlag / doRaceFlag, doSetupRacePods /
// doRacePods and doSetupOpponent / doOpponent in
// BehaviorScript 4 - PlayerCar.ls, for all five races. The flag nearest the
// player is the active race; race i is against car i + 1 and wins it.
//
// raceState: 0 idle at the flag, 1 on the start line (Ready / Set / Go),
// 2 racing, 3 won, 4 cooling down after a win, 6 lost, 7 wrecked.
#include "car.hpp"
#include "math.hpp"

namespace rr4 {

class World;
class Renderer;
struct Camera;

// Opponent path (redDotPosList) and the red dots that become race pods
// (racePodPosList), from the MapData cast. Generated: src/race_data.gen.cpp.
struct RacePath {
    const Vec3 *dots;
    int dotCount;
    const int *checkpoints;   // 1-based indices into dots
    int checkpointCount;
};
extern const RacePath kRacePaths[5];   // races 1..5

class Race {
public:
    void setup();
    // doOpponent: runs before the player's own handlers in enterFrame.
    void updateOpponent(Car &player, const World &world);
    // doRaceFlag then doRacePods, after doIgNitro.
    void updateFlag(Car &player, World &world);
    // doFerrisWheel: driving back to the wheel mid-race abandons it.
    void abandon(Car &player, World &world);
    void draw(Renderer &r, const Camera &cam) const;

    int state() const { return state_; }

private:
    struct Opponent {
        Car shell;                 // drawing state: steer, wheelSpin, wheelImbed
        Mat4 rot = Mat4::identity();
        Vec3 rotVel;               // opponentRotVector, degrees per frame
        Vec3 prevVel;              // prevOpponentVelVector
        float hoist = 0.0f;
        bool midAir = true;
        bool inWorld = false;
        int state = 0;             // opponentState
        int dotID = 1;             // opponentDotID (1-based)
        Vec3 target;
        float targetVel = 100.0f;
        float throttle = 0.0f;
        float imbed[4] = {};
        Vec3 normal[4];
    };

    void selectRace(int race);
    void spawnOpponent();
    void opponentGround(const World &world);
    void placePods();
    void hideRaceObjects(World &world);
    void returnToFerris(Car &player, World &world);
    Vec3 podPos(int pointer) const;   // racePodPosList[currentRace][pointer]

    const RacePath *path_ = nullptr;
    Vec3 flagPos_;
    float flagYawDeg_ = 0.0f;
    int state_ = 0;
    int currentRace_ = 1, prevCurrentRace_ = 0;
    bool opponentSpawned_ = false;
    long ticks_ = 0;
    long flagStartTicks_ = 0;
    long lightStartTicks_ = 0;
    int lightPhase_ = 0;
    float lightsU_ = 0.0f;           // tRaceFlagLights.textureTransform.position.x
    float arrowsV_ = 0.0f;           // tRaceFlagArrows scroll (port V)
    int podPointer_ = 1;             // racePodPointer
    bool podVisible_ = false, nextPodVisible_ = false;
    Vec3 podAt_, nextPodAt_;
    float podYaw_ = 0.0f, nextPodYaw_ = 0.0f;   // radians
    float mistU_ = 0.0f, mistV_ = 0.0f;
    int racesWon_ = 0;               // g_NumberRacesWon
    Opponent opp_;
};

} // namespace rr4
