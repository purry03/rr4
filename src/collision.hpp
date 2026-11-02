#pragma once
// Original Redline Rumble 4 tile collisions.
//
// `doCollisions` in BehaviorScript 4 - PlayerCar.ls does not use boxes or
// swept shapes: for the four tiles around the car it builds a tile-local
// `pushOrigin` and calls `doRadialPush`, which shoves the car out of a circle
// centred on that point. The clamps that build `pushOrigin` are hand-tuned per
// tile, so the switch is generated from the decompiled Lingo by
// tools/gen_collisions.py into src/collision_cases.gen.cpp.
//
// This header models one call of doCollisions: the car state, the four tile
// origins and the shared globals the original reads (g_FPSFactor, the plane
// collider and the airport radar's spinning x-axis).

#include "math.hpp"

namespace rr4 {

struct Car;
struct World;

struct CollisionCtx {
    Vec3 carPos;      // gPlayerCar.worldPosition at the start of the pass
    Vec3 tilePos;     // mVisTileList[tempTile].worldPosition (tile corner)
    Vec3 origin;      // pushOrigin (tile-local)
    Vec3 planeLocal;  // gPlaneCollider.transform.position (relative to mPlane)
    Mat4 plane = Mat4::identity();   // mPlane world transform (rigid)
    Vec3 radarXAxis;  // mRadar.transform.xAxis
    Vec3 vel;         // velVector
    Vec3 rot;         // rotVector (x pitch, y yaw, z roll, degrees per frame)
    float radius = 140.0f;  // tempRadius, persists across tiles
    float fps = 1.0f;       // g_FPSFactor
    float damage = 0.0f;    // g_DamagePercent
    float impact = 0.0f;    // largest push magnitude, for CrashTinkle

    Vec3 planeWorld() const { return plane.transformPoint(planeLocal); }
    // Setting gPlaneCollider.worldPosition: back into mPlane's frame.
    Vec3 planeToLocal(const Vec3 &world) const {
        Vec3 d = world - Vec3(plane.m[12], plane.m[13], plane.m[14]);
        return {d.x * plane.m[0] + d.y * plane.m[1] + d.z * plane.m[2],
                d.x * plane.m[4] + d.y * plane.m[5] + d.z * plane.m[6],
                d.x * plane.m[8] + d.y * plane.m[9] + d.z * plane.m[10]};
    }

    // doRadialPush()
    void push(float radius, const Vec3 &origin, int zeroY, int playSfx);
};

// One `case tempTileID of` arm. `onTile` is the car's own tile index (the
// original switches on it for the bridge and canal variants).
void collideTile(CollisionCtx &ctx, int tileID, int tileRot, int onTile);

// doCollisions tile pass for the car's current position (four nearest tiles).
// The airport tiles read the plane and radar state from `world`.
void resolveTileCollisions(const World &world, Car &car);

} // namespace rr4
