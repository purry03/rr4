#include "collision.hpp"
#include "car.hpp"
#include "world.hpp"

#include <algorithm>
#include <cmath>

namespace rr4 {

namespace {
// mRadar is the airport radar dish; doUpdate rotates it 5 degrees per frame.
float gRadarAngle = 0.0f;

// mPlane starts parked at airPortOrigin (doSetupPlane). The plane collider is
// a child of the plane, so its local position is relative to this point.
const Vec3 kPlaneOrigin{7500.0f, 0.0f, 7500.0f};
} // namespace

void collideAdvanceFrame(float fps) {
    gRadarAngle += 5.0f * fps;
    if (gRadarAngle > 360.0f) gRadarAngle -= 360.0f;
}

Vec3 collideRadarXAxis() {
    float a = deg2rad(gRadarAngle);
    return {std::cos(a), 0.0f, -std::sin(a)};
}

void CollisionCtx::push(float r, const Vec3 &o, int zeroY, int playSfx) {
    // diffVector = tilePos + pushOrigin - (carPos + velVector * g_FPSFactor)
    Vec3 diff = tilePos + o - (carPos + vel * fps);
    if (zeroY) diff.y = 0.0f;
    float mag = diff.length();
    if (mag >= r) return;

    Vec3 pushVector = diff.normalized() * (r - mag);
    vel = vel - (pushVector / fps);
    rot.z = rot.z - (pushVector.x * 0.4f / fps);
    rot.x = rot.x + (pushVector.z * 0.4f / fps);

    float pushMag = pushVector.length() / fps;
    impact = std::max(impact, pushMag);
    if (!playSfx) return;
    // CrashTinkle thresholds from doRadialPush.
    if (pushMag > 20.0f) damage += 0.02f;
    else if (pushMag > 15.0f) damage += 0.015f;
    else if (pushMag > 10.0f) damage += 0.01f;
    else if (pushMag > 5.0f) damage += 0.005f;
}

void resolveTileCollisions(const World &world, Car &car) {
    // doUpdate() spins the airport radar 5 degrees per frame.
    collideAdvanceFrame(1.0f);
    // onTileCol = integer(x / 5000 + 0.5), onTileRow likewise.
    int col = world.tileColAt(car.pos.x);
    int row = world.tileRowAt(car.pos.z);

    // onTileX = worldPosition.x mod 5000, always in [0, 5000).
    float onTileX = std::fmod(car.pos.x, TILE_SIZE);
    if (onTileX < 0.0f) onTileX += TILE_SIZE;
    float onTileZ = std::fmod(car.pos.z, TILE_SIZE);
    if (onTileZ < 0.0f) onTileZ += TILE_SIZE;

    // visTileCollisionList: the car's tile plus the three neighbours around
    // the tile corner the car is closest to, in the original order.
    int nearCol = onTileX < 2500.0f ? col - 1 : col + 1;
    int nearRow = onTileZ < 2500.0f ? row - 1 : row + 1;
    const int listCol[4] = {col, nearCol, col, nearCol};
    const int listRow[4] = {row, row, nearRow, nearRow};

    // The original switches several arms on the car's own tile index (1-based).
    int onTile = 1;
    if (col >= 1 && col <= GRID && row >= 1 && row <= GRID)
        onTile = (col - 1) + (row - 1) * GRID + 1;

    CollisionCtx ctx;
    // doCollisions runs before doupdate() in the original, so carPos is the
    // pre-move position and carPos + velVector * g_FPSFactor is the spot the
    // car is about to occupy. doupdate() then moves the pushed velocity there.
    ctx.carPos = car.pos;
    ctx.vel = car.vel;
    ctx.rot = Vec3(car.rotXDeg, car.yawVelDeg, car.rotZDeg);
    ctx.damage = car.damage;
    ctx.radius = 140.0f;
    ctx.fps = 1.0f;
    ctx.planePos = kPlaneOrigin;
    ctx.radarXAxis = collideRadarXAxis();

    for (int i = 0; i < 4; ++i) {
        int c = listCol[i], rw = listRow[i];
        // Out-of-range tiles have visTileIDList = 0, which hits no case.
        if (c < 1 || c > GRID || rw < 1 || rw > GRID) continue;
        int idx = (c - 1) + (rw - 1) * GRID;
        int id = kTileID[idx];
        if (id <= 0) continue;
        // mVisTileList[tempTile].worldPosition is the tile centre.
        ctx.tilePos = World::tileCenter(c, rw);
        collideTile(ctx, id, kTileRot[idx], onTile);
    }

    car.vel = ctx.vel;
    car.rotXDeg = ctx.rot.x;
    car.yawVelDeg = ctx.rot.y;
    car.rotZDeg = ctx.rot.z;
    car.damage = ctx.damage;
    // Audio::tickCar plays CrashTinkle from impactSpeed at the same 5/10/15/20
    // thresholds doRadialPush uses.
    car.impactSpeed = std::max(car.impactSpeed, ctx.impact);
}

} // namespace rr4
