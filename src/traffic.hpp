#pragma once
// Original traffic: doSetupTraffic / doTraffic / doTrafficLights and the
// traffic loop at the top of doCollisions (BehaviorScript 4 - PlayerCar.ls).
//
// Traffic cars follow the MapData dot network. Every dot holds at most one
// car (dotOccupiedList); traffic lights mark their dots -1111 while red, so a
// car arriving there waits until the light changes.

#include "math.hpp"
#include "renderer.hpp"

#include <cstdint>
#include <vector>

namespace rr4 {

struct Car;
struct World;

// MapData member 1 (tools/gen_race_data.py, src/traffic_data.gen.cpp). All
// ids are the original 1-based ones; <list>Start has count + 1 offsets into
// <list>Ids.
struct TrafficMap {
    const Vec3 *dotPos;
    int dotCount;
    const int *nextStart, *nextIds;   // nextDotIDList
    const int *prevStart, *prevIds;   // prevDotIDList
    const int *tileStart, *tileIds;   // tileDotIDList, per tile index
    int tileCount;
    const int *redLights;             // redTrafficLightIDList
    int redCount;
    const int *greenLights;           // greenTrafficLightIDList
    int greenCount;
};
extern const TrafficMap kTrafficMap;

class Traffic {
public:
    // doSetupTraffic + doSetupTrafficLights.
    void setup();
    // doTraffic: spawn around the focus (the player, or the menu camera),
    // drive, cull. `ticks` is the
    // shared 60 Hz frame counter (the ticks).
    void update(const World &world, const Vec3 &focus, long ticks);
    // doTrafficLights: cycle the two light phases and their dots.
    void updateLights(long ticks);
    // doCollisions: radius 140 push of the player away from every live car.
    void collide(Car &player) const;
    void draw(Renderer &r, const Camera &cam) const;
    // trafficSpawnedLimit (5; stage 5 raises it to 20).
    void setSpawnLimit(int n) { spawnLimit_ = n; }

    struct Slot {
        int state = 0;            // trafficStateList: 0 parked, 1 driving
        Vec3 pos{-1000, -1000, -1000};
        float yaw = 0.0f;         // group rotation about Y (radians)
        Mat4 body = Mat4::identity();   // child model world rotation
        Vec3 target;              // trafficTargetPosList
        float speed = 0.0f, targetSpeed = 0.0f;
        int dot = 0;              // trafficDotIDList (1-based, 0 = none)
    };
    const std::vector<Slot> &cars() const { return slots_; }

private:
    int &occupied(int dot) { return occupied_[size_t(dot - 1)]; }
    int random(int n);           // Lingo random(n): 1..n

    std::vector<Slot> slots_;
    std::vector<int> occupied_;  // dotOccupiedList
    std::vector<long> spawnTicks_;   // trafficDotSpawnTicksList
    int spawned_ = 0;            // trafficSpawnedCounter
    int spawnLimit_ = 5;         // trafficSpawnedLimit
    int lightPhase_ = 1;         // trafficLightPhase
    long lightStartTicks_ = 0;   // trafficLightStartTicks
    uint32_t seed_ = 0x7a3f19c5u;
    int mesh_ = -1;              // TrafficCar_000
};

} // namespace rr4
