#include "race.hpp"
#include "garage.hpp"
#include "assets.hpp"
#include "world.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace rr4 {

namespace {

constexpr int kRaces = 5;
// doSetupRaceFlag: raceFlagPosList / raceFlagRotList.
const Vec3 kFlagPos[kRaces] = {{32501.53f, -2.69f, 24293.58f}, {50049.06f, 14.44f, 29431.89f},
                               {52984.54f, 14.47f, 38827.75f}, {31150.03f, -5.56f, 42488.2f},
                               {19312.33f, 14.44f, 29473.48f}};
const float kFlagYaw[kRaces] = {0.0f, 270.0f, 180.0f, 90.0f, 90.0f};
// doSetupigNitro: igNitroRacePosList, shown 55 above these points.
const std::vector<Vec3> kRaceNitro[kRaces] = {
    {{42600.58f, -5.48f, 12511.97f}, {50070.05f, 14.61f, 29341.57f}},
    {{13105.45f, -5.56f, 43014.47f}, {14760.87f, 14.2f, 43129.26f}},
    {{47507.21f, -5.55f, 11486.69f}, {40673.16f, 14.44f, 29494.3f}},
    {{38064.52f, -5.48f, 52491.06f}},
    {{32052.29f, 75.18f, 51194.55f}, {43737.1f, 20.02f, 43158.14f}, {2624.11f, -5.56f, 33092.95f}},
};
// doRaceFlag case 3 / 6: the Ferris' Wheels spawn the player returns to.
const Vec3 kFerrisSpawn{32053.57f, -5.56f, 30797.51f};
// doSetupPointPods: mPointPod.worldPosition, which the last race pod faces
// away from.
const Vec3 kPointPodPos{12500.0f, 0.0f, 4000.0f};
// doOpponent: opponent acceleration, case currentRace + 1 = 2..6.
const float kOpponentAccel[kRaces] = {0.03f, 0.032f, 0.036f, 0.038f, 0.04f};
// gOpponentCageList: the four wheel ray points.
const Vec3 kOppCage[4] = {{-50, 30, -60}, {50, 30, -60}, {-50, 30, 60}, {50, 30, 60}};

int gFlagMesh = -1, gPodMesh = -1;

// Director pointAt: the model's front is -Z, so -Z ends up facing `to`.
} // namespace

// doRadialPush-style shove of the player away from a point.
float pushPlayer(Car &player, const Vec3 &centre, float radius, float rotScale) {
    Vec3 diff = centre - (player.pos + player.vel);
    float mag = diff.length();
    if (mag >= radius || mag < 1e-4f) return 0.0f;
    Vec3 push = diff.normalized() * (radius - mag);
    player.vel -= push;
    player.rotZDeg -= push.x * rotScale;
    player.rotXDeg += push.z * rotScale;
    return push.length();
}

void flagPolePush(Car &player, const Vec3 &pole) {
    float mag = pushPlayer(player, pole, 120.0f, 0.4f);
    if (mag > 20.0f) player.damage += 0.04f;
    else if (mag > 15.0f) player.damage += 0.03f;
    else if (mag > 10.0f) player.damage += 0.02f;
    else if (mag > 5.0f) player.damage += 0.01f;
    player.impactSpeed = std::max(player.impactSpeed, mag);
}


void Race::setup() {
    selectRace(1);
    gFlagMesh = gAssets.meshIndex("RaceFlag_001");
    gPodMesh = gAssets.meshIndex("RacePod_001");
}

// The flag moves to race `race`; its opponent is mOpponentShellList[race],
// car race + 1.
void Race::selectRace(int race) {
    path_ = &kRacePaths[race - 1];
    flagPos_ = kFlagPos[race - 1];
    flagYawDeg_ = kFlagYaw[race - 1];
    opp_.shell.setCar(race + 1);
}

Vec3 Race::radarFlag(const int carsOwned[kCarCount]) const {
    int race = 0;
    for (int i = 6; i >= 2; --i)
        if (carsOwned[i - 1] == kNotOwned) race = i - 1;
    return race == 0 ? flagPos_ : kFlagPos[race - 1];
}

Vec3 Race::podPos(int pointer) const {
    return path_->dots[path_->checkpoints[pointer - 1] - 1];
}

void Race::spawnOpponent() {
    // flag + (0, 5, 0) + zAxis * 140 - xAxis * 140, facing back past the flag.
    Mat4 flagRot = Mat4::rotationY(deg2rad(flagYawDeg_));
    Vec3 x = flagRot.transformVector({1, 0, 0}), z = flagRot.transformVector({0, 0, 1});
    opp_.shell.pos = flagPos_ + Vec3(0, 5, 0) + z * 140.0f - x * 140.0f;
    opp_.rot = Mat4::rotationY(deg2rad(flagYawDeg_ + 180.0f));
    opp_.shell.vel = opp_.rotVel = opp_.prevVel = {};
    opp_.inWorld = true;
}

void Race::placePods() {
    podAt_ = podPos(podPointer_);
    nextPodAt_ = podPos(podPointer_ + 1);
    podYaw_ = pointAtYaw(podAt_, nextPodAt_);
    nextPodYaw_ = pointAtYaw(nextPodAt_, podAt_);
    podVisible_ = nextPodVisible_ = true;
}

void Race::hideRaceObjects(World &world) {
    podVisible_ = nextPodVisible_ = false;
    for (World::NitroPickup &p : world.nitroPickups) p.visible = false;
}

// doRaceFlag case 3 (and case 6 with a car to give back): back to the
// Ferris' Wheels spawn with the race torn down.
void Race::returnToFerris(Car &player, World &world) {
    player.pos = kFerrisSpawn;
    player.setRotationDeg({0.0f, 45.0f, 0.0f});
    player.resetParts();
    player.vel = {};
    player.yawVelDeg = player.rotXDeg = player.rotZDeg = 0.0f;
    player.raceReturn = true;
    currentRace_ = 0;
    opp_.state = 0;
    opp_.dotID = 0;
    opp_.inWorld = false;
    opp_.shell.pos = {-1000, -1000, -1000};
    hideRaceObjects(world);
}

void Race::abandon(Car &player, World &world) {
    player.doPanel(33, 200);
    currentRace_ = 0;
    opp_.state = 0;
    state_ = 0;
    podPointer_ = 0;
    opp_.dotID = 0;
    opp_.shell.vel = opp_.rotVel = {};
    opp_.inWorld = false;
    opp_.shell.pos = {-1000, -1000, -1000};
    hideRaceObjects(world);
    player.timerReset = true;
    player.timerState = 0;
    lightPhase_ = 0;
    flagStartTicks_ = ticks_;
    opponentSpawned_ = false;
}

// The four-ray suspension of doOpponent, using the tile height field.
void Race::opponentGround(const World &world) {
    opp_.hoist = 0.0f;
    opp_.midAir = true;
    float cap[4];
    Vec3 toCenter[4];
    for (int i = 0; i < 4; ++i) {
        Vec3 local = opp_.rot.transformVector(kOppCage[i]);
        Vec3 p = opp_.shell.pos + local;
        float reach = p.y + 60.0f - 140.0f;
        toCenter[i] = local;
        float y;
        if (!world.groundRayAt(p.x, p.z, reach, y)) {
            // Ray missed (beside the sunken airport tunnel floor): no imbed.
            opp_.imbed[i] = 0.0f;
            cap[i] = 0.0f;
            continue;
        }
        opp_.imbed[i] = std::max(0.0f, y - p.y + 30.0f);
        if (y - p.y + 30.0f >= 0.0f) opp_.midAir = false;
        opp_.normal[i] = world.groundNormalAt(p.x, p.z, reach);
        cap[i] = opp_.imbed[i];
        if (cap[i] > 20.0f) {
            opp_.hoist = std::max(opp_.hoist, opp_.imbed[i] - 20.0f);
            cap[i] = 20.0f;
        }
    }
    for (int i = 0; i < 4; ++i) {
        opp_.shell.vel.y += cap[i] * 0.045f;
        opp_.rotVel.x -= toCenter[i].z * cap[i] * 0.0008f;
        opp_.rotVel.z += toCenter[i].x * cap[i] * 0.0008f;
        if (opp_.imbed[i] > 0.0f) {
            opp_.shell.vel.z += opp_.normal[i].z * 0.1f;
            opp_.shell.vel.x += opp_.normal[i].x * 0.1f;
        }
    }
}

void Race::updateOpponent(Car &player, const World &world) {
    Car &o = opp_.shell;
    if (opp_.state == 1) {
        opponentGround(world);

        // Head for the next red dot; the speed target is the dot spacing.
        Vec3 diff = o.pos - opp_.target;
        diff.y = 0.0f;
        if (diff.length() < 800.0f) {
            Vec3 previous = opp_.target;
            ++opp_.dotID;
            if (path_->dotCount >= opp_.dotID) {
                opp_.target = path_->dots[opp_.dotID - 1];
                opp_.targetVel = (previous - opp_.target).length() * 0.04f;
            }
        }
        const Vec3 xAxis = opp_.rot.transformVector({1, 0, 0});
        const Vec3 zAxis = opp_.rot.transformVector({0, 0, 1});
        float steer = xAxis.dot(-diff);
        float behind = zAxis.dot(-diff);
        if (behind < 0.0f) steer = steer > 0.0f ? 1.0f : -1.0f;
        if (steer > 0.2f) {
            if (o.steer < 0) o.steer = 0;
            if (o.steer < 40) o.steer += 2.0f;
        } else if (steer < -0.2f) {
            if (o.steer > 0) o.steer = 0;
            if (o.steer > -40) o.steer -= 2.0f;
        } else {
            o.steer *= 0.9f;
        }
        if (o.vel.length() < opp_.targetVel) opp_.throttle += 0.2f;
        else opp_.throttle = 0.0f;
        opp_.throttle = clampf(opp_.throttle, -8.0f, 10.0f);
        if (behind < 0.0f) opp_.throttle = 2.0f;
        if (state_ > 2) {
            opp_.throttle = 0.0f;
            o.steer = 0.0f;
            o.vel -= o.vel * 0.05f;
        }
        if (!opp_.midAir) o.vel += zAxis * (opp_.throttle * kOpponentAccel[opp_.shell.carId - 2]);
        float steerVel = clampf(o.steer * 0.001f * -zAxis.dot(o.vel), -0.5f, 0.5f);
        if (opp_.imbed[0] > 0.0f || opp_.imbed[1] > 0.0f) opp_.rotVel.y -= steerVel;
        o.vel.y -= 1.0f;
        // Deliberate deviation: doOpponent tests the player's midAir here (a
        // copy-paste slip from doFriction), so the opponent was damped
        // whenever the player was grounded. Use the opponent's own state, as
        // its throttle and grip checks already do.
        if (!opp_.midAir) {
            o.vel.y -= o.vel.y * 0.05f;
            o.vel -= o.vel * 0.005f;
            opp_.rotVel -= opp_.rotVel * 0.16f;
        }
        Vec3 grip = xAxis * o.vel.dot(xAxis);
        if (grip.length() > 1.0f) grip = grip.normalized();
        if (!opp_.midAir) o.vel -= grip;
        Vec3 roll = o.vel - opp_.prevVel;
        roll.y = 0.0f;
        if (roll.length() > 1.0f) roll = roll.normalized();
        opp_.rotVel.x -= roll.z * 0.15f;
        opp_.rotVel.z += roll.x * 0.15f;

        o.pos += o.vel + Vec3(0.0f, opp_.hoist, 0.0f);
        // mOpponentCar.rotate(opponentRotVector, #parent)
        opp_.rot = Mat4::rotationZ(deg2rad(opp_.rotVel.z)) * Mat4::rotationY(deg2rad(opp_.rotVel.y)) *
                   Mat4::rotationX(deg2rad(opp_.rotVel.x)) * opp_.rot;

        Vec3 rollVec = o.vel * 1.6f;
        rollVec.y = 0.0f;
        float rollFactor = rollVec.length();
        float rollVel = rollFactor > 1e-4f ? (rollVec / rollFactor).dot(zAxis) * rollFactor : 0.0f;
        o.wheelSpin += rollVel + opp_.throttle * 2.0f;
        for (int i = 0; i < 4; ++i) o.wheelImbed[i] = opp_.imbed[i];
        opp_.prevVel = o.vel;
    }

    // Bumping into the opponent (radius 160) pushes the player.
    if (opp_.inWorld) {
        Vec3 centre = o.pos + o.vel;
        float mag = pushPlayer(player, centre, 160.0f, 0.2f);
        if (mag > 5.0f) { player.damage += 0.0005f; player.impactSpeed = std::max(player.impactSpeed, 5.0f); }
        if (mag > 15.0f) { player.damage += 0.001f; player.impactSpeed = std::max(player.impactSpeed, 15.0f); }
    }
}

void Race::updateFlag(Car &player, World &world) {
    ++ticks_;

    // doupdate: maxing out the damage mid-race fails it (panel 30 already
    // requested by the car).
    if (player.raceWrecked) {
        player.raceWrecked = false;
        state_ = 7;
        lightPhase_ = 0;
        flagStartTicks_ = ticks_;
        podVisible_ = nextPodVisible_ = false;
    }

    // Start-light texture: one quarter per phase.
    const float lightTarget = -0.25f * float(lightPhase_);
    if (lightPhase_ <= 3 && lightsU_ > lightTarget) lightsU_ -= 0.05f;

    Mat4 flagRot = Mat4::rotationY(deg2rad(flagYawDeg_));
    const Vec3 xAxis = -flagRot.transformVector({1, 0, 0});
    const Vec3 zAxis = -flagRot.transformVector({0, 0, 1});
    const Vec3 carZ = player.forward();
    Vec3 diff = flagPos_ - (player.pos + player.vel);
    const float xDist = xAxis.dot(diff), zDist = zAxis.dot(diff);
    const bool inBox = xDist > 0 && xDist < 250 && zDist > 0 && zDist < 400 &&
                       zAxis.dot(carZ) > 0.75f;
    const float forwardVel = zAxis.dot(player.vel);

    switch (state_) {
    case 0: {
        // The nearest flag is the active race.
        float best = 1e30f;
        for (int i = 0; i < kRaces; ++i) {
            float d = (player.pos - kFlagPos[i]).length();
            if (d < best) { best = d; currentRace_ = i + 1; }
        }
        if (currentRace_ != prevCurrentRace_) {
            selectRace(currentRace_);
            opponentSpawned_ = false;
        }
        if (!opponentSpawned_ && (player.pos - flagPos_).length() < 5000.0f) {
            opponentSpawned_ = true;
            spawnOpponent();
        }
        arrowsV_ += 0.02f;   // translate(0, -0.02, 0)
        if (flagShown() && world.stage.state() == 0 && diff.length() < 600.0f && inBox) {
            float prevZ = zAxis.dot(flagPos_ - player.pos);
            if (zDist < 100 && forwardVel > 0 && prevZ >= 100) {
                // Over the start line: stop on it and line up.
                Vec3 push = -zAxis * (100.0f - zDist) - xAxis * ((160.0f - xDist) * 0.1f);
                podPointer_ = 1;
                player.vel += push;
                player.doPanel(18, 0);          // Ready
                player.timerReset = true;
                player.timerState = 0;
                state_ = 1;
                player.sfx.push_back(26);       // RaceStartFanFair
                lightStartTicks_ = ticks_;
                lightPhase_ = 1;
                lightsU_ = 0.0f;
                arrowsV_ = 0.0f;
                placePods();
                const std::vector<Vec3> &bottles = kRaceNitro[currentRace_ - 1];
                for (size_t i = 0; i < world.nitroPickups.size(); ++i) {
                    World::NitroPickup &p = world.nitroPickups[i];
                    p.visible = i < bottles.size();
                    if (p.visible) p.pos = bottles[i] + Vec3(0, 55, 0);
                }
            }
        }
        break;
    }
    case 1:
        if (diff.length() < 600.0f) {
            if (inBox) {
                if (zDist < 100 && forwardVel > 0) player.vel += -zAxis * (100.0f - zDist);
                player.yawVelDeg -= 2.0f * xAxis.dot(carZ);
            } else {
                player.doPanel(28, 200);        // RACE CANCELLED
                state_ = 0;
                lightPhase_ = 0;
                podVisible_ = nextPodVisible_ = false;
                for (World::NitroPickup &p : world.nitroPickups) p.visible = false;
            }
            if (ticks_ - lightStartTicks_ > 60) {
                lightStartTicks_ = ticks_;
                ++lightPhase_;
                player.doPanel(17 + lightPhase_, 0);   // Set, Go!
                if (lightPhase_ >= 3) {
                    opp_.state = 1;
                    opp_.target = path_->dots[0];
                    player.timerState = -2;
                    podPointer_ = 1;
                    state_ = 2;
                    player.doPanel(20, 100);
                }
            }
        }
        break;
    case 2:
        if (podPointer_ > path_->checkpointCount) {
            state_ = 3;
            ++racesWon_;
            opp_.dotID = 0;
            player.timerState = 0;
            flagStartTicks_ = ticks_;
            lightPhase_ = 0;
            int &prize = world.carsOwned[size_t(currentRace_)];   // car currentRace + 1
            if (prize == kNotOwned) {
                player.doPanel(34, 0);          // RACE WINNER! You won your opponents car
                prize = kOwnedParked;
                player.saveGame = true;
            } else {
                player.doPanel(44, 0);          // ... and already own this car
            }
            // puppetSound(1, member(34, "Sfx")): that Sfx slot is empty.
        } else if (opp_.dotID > path_->dotCount) {
            player.timerState = 0;
            opp_.dotID = 0;
            flagStartTicks_ = ticks_;
            lightPhase_ = 0;
            player.doPanel(player.carId > 1 ? 37 : 39, 0);
            state_ = 6;
            player.sfx.push_back(28);           // RaceStopFanFair
        }
        break;
    case 3:
        if (ticks_ - flagStartTicks_ > 300) {
            state_ = 4;
            player.doPanel(35, 1);
            flagStartTicks_ = ticks_;
            player.fadeToBlack = true;
            world.ferris.reset();
            returnToFerris(player, world);
        }
        break;
    case 4:
        if (ticks_ - flagStartTicks_ > 300) state_ = 0;
        break;
    case 6:
        if (ticks_ - flagStartTicks_ > 400) {
            state_ = 0;
            player.doPanel(38, 300);            // RACE LOST! Returning you ...
            flagStartTicks_ = ticks_;
            player.fadeToBlack = true;
            player.damage = 0.0f;
            player.boostMeter = 1.0f;
            world.ferris.reset();
            if (player.carId > 1) {
                // The opponent keeps your car; drive the highest one left.
                world.carsOwned[size_t(player.carId - 1)] = kNotOwned;
                player.saveGame = true;
                int id = 1;
                for (int i = 0; i < kCarCount; ++i)
                    if (world.carsOwned[size_t(i)] != kNotOwned) id = i + 1;
                world.carsOwned[size_t(id - 1)] = kOwnedDriving;
                player.setCar(id);
                world.ferris.reset();
                returnToFerris(player, world);
            }
        }
        break;
    case 7:
        if (ticks_ - flagStartTicks_ > 300) {
            state_ = 0;
            opponentSpawned_ = false;
            opp_.dotID = 0;
            currentRace_ = 0;
            opp_.state = 0;
            opp_.inWorld = false;
            hideRaceObjects(world);
        }
        break;
    }

    // The flag pole itself is solid (radius 120) while the flag is out.
    if (flagShown()) flagPolePush(player, flagPos_);

    // doRacePods
    if (state_ != 0 && state_ != 6) {
        if ((player.pos - podAt_).length() < 800.0f) {
            ++podPointer_;
            if (podPointer_ < path_->checkpointCount) {
                player.sfx.push_back(29);       // TimeBonusCountUp
                placePods();
            } else if (podPointer_ == path_->checkpointCount) {
                player.sfx.push_back(29);
                podAt_ = podPos(podPointer_);
                podYaw_ = pointAtYaw(podAt_, kPointPodPos) + PI;
                nextPodVisible_ = false;
            } else {
                podVisible_ = false;
            }
        }
        mistU_ += 0.025f;   // translate(0.025, -0.05, 0)
        mistV_ += 0.05f;
    } else if (state_ == 6) {
        podVisible_ = nextPodVisible_ = false;
    }

    player.inRace = state_ > 0;
    prevCurrentRace_ = currentRace_;
    // The flag leaves with the race and comes back 5 s after it ends.
    if (prevState_ != 0 && state_ == 0) flagReadyTicks_ = ticks_ + 300;
    prevState_ = state_;
}

void Race::draw(Renderer &r, const Camera &cam) const {
    if (gFlagMesh >= 0 && flagShown()) {
        // Shaders 3 and 4: the start lights and the scrolling arrows. A
        // Director textureTransform position p samples the texture at uv - p
        // (V is flipped in the port, so the V shift keeps its sign there).
        Mat4 m = Mat4::translation(flagPos_) * Mat4::rotationY(deg2rad(flagYawDeg_));
        drawAssetGroups(r, cam, m, gAssets.meshes[size_t(gFlagMesh)],
                   [&](int g, uint32_t &, float &du, float &dv) {
                       if (g == 2) du = -lightsU_;
                       if (g == 3) dv = -arrowsV_;
                       return true;
                   });
    }
    if (gPodMesh >= 0) {
        const AssetMesh &pod = gAssets.meshes[size_t(gPodMesh)];
        if (podVisible_)
            drawAssetGroups(r, cam, Mat4::translation(podAt_) * Mat4::rotationY(podYaw_), pod,
                       [&](int g, uint32_t &, float &du, float &dv) {
                           if (g == 2) { du = -mistU_; dv = -mistV_; }
                           return true;
                       });
        // The next pod: shaders 1 and 2 dark purple without emission, the
        // mist (shader 3) at blend 0.
        if (nextPodVisible_)
            drawAssetGroups(r, cam, Mat4::translation(nextPodAt_) * Mat4::rotationY(nextPodYaw_), pod,
                       [&](int g, uint32_t &tint, float &, float &) {
                           tint = packRGBA(50, 0, 50);
                           return g != 2;
                       });
    }
    if (opp_.inWorld)
        opp_.shell.drawShell(r, cam, Mat4::translation(opp_.shell.pos) * opp_.rot);
}

} // namespace rr4
