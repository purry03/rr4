#include "stage.hpp"
#include "assets.hpp"
#include "car.hpp"
#include "hud.hpp"
#include "world.hpp"

#include <cmath>

namespace rr4 {

namespace {

// Countdown minutes per stage (timeList = [0, 0, 0, 0, n]).
const int kStageMinutes[8] = {2, 2, 3, 4, 5, 5, 5, 4};
// igNitroStagePosList: the same three bottles for every stage, 55 up.
const Vec3 kStageNitro[3] = {{32052.29f, 75.18f, 51194.55f},
                             {42533.07f, 632.62f, 53121.95f},
                             {43737.10f, 20.02f, 43158.14f}};
int gFlagMesh = -1, gPodMesh = -1;

} // namespace

void Stage::setup() {
    gFlagMesh = gAssets.meshIndex("PointsFlag_001");
    gPodMesh = gAssets.meshIndex("PointsPod_001");
}

void Stage::placePods() {
    const StageDef &st = kStages[current_ - 1];
    podAt_ = st.pods[podPointer_ - 1];
    nextPodAt_ = st.pods[podPointer_];
    podYaw_ = pointAtYaw(podAt_, nextPodAt_);
    nextPodYaw_ = pointAtYaw(nextPodAt_, podAt_);
    podVisible_ = nextPodVisible_ = true;
}

void Stage::hidePods(World &world) {
    podVisible_ = nextPodVisible_ = false;
    for (World::NitroPickup &p : world.nitroPickups) p.visible = false;
}

void Stage::update(Car &player, World &world) {
    ++ticks_;
    const StageDef &st = kStages[current_ - 1];

    if (player.stageWrecked) {   // doupdate: wrecked mid-stage
        player.stageWrecked = false;
        state_ = 7;
        lightPhase_ = 0;
        startTicks_ = ticks_;
        hidePods(world);
        world.traffic.setSpawnLimit(5);
    }

    const float lightTarget = -0.25f * float(lightPhase_);
    if (lightPhase_ <= 3 && lightsU_ > lightTarget) lightsU_ -= 0.05f;

    Mat4 flagRot = Mat4::rotationY(deg2rad(st.flagYawDeg));
    const Vec3 xAxis = -flagRot.transformVector({1, 0, 0});
    const Vec3 zAxis = -flagRot.transformVector({0, 0, 1});
    const Vec3 carZ = player.forward();
    Vec3 diff = st.flag - (player.pos + player.vel);
    const float xDist = xAxis.dot(diff), zDist = zAxis.dot(diff);
    const bool inBox = xDist > 0 && xDist < 250 && zDist > 0 && zDist < 400 &&
                       zAxis.dot(carZ) > 0.75f;
    const float forwardVel = zAxis.dot(player.vel);

    switch (state_) {
    case 0: {
        // The nearest start is the active stage.
        float best = 1e30f;
        for (int i = 0; i < 8; ++i) {
            float d = (player.pos - kStages[i].flag).length();
            if (d < best) { best = d; current_ = i + 1; }
        }
        if (world.race.state() > 0) arrowsV_ = 0.0f;
        else arrowsV_ += 0.01f;   // translate(0, -0.01, 0)
        const StageDef &s = kStages[current_ - 1];
        Mat4 rot = Mat4::rotationY(deg2rad(s.flagYawDeg));
        const Vec3 x = -rot.transformVector({1, 0, 0}), z = -rot.transformVector({0, 0, 1});
        Vec3 d = s.flag - (player.pos + player.vel);
        const float xd = x.dot(d), zd = z.dot(d);
        if (!flagShown() || world.race.state() != 0 || d.length() >= 600.0f) break;
        if (!(xd > 0 && xd < 250 && zd > 0 && zd < 400 && z.dot(carZ) > 0.75f)) break;
        const float prevZ = z.dot(s.flag - player.pos);
        if (zd < 100 && z.dot(player.vel) > 0 && prevZ >= 100) {
            // Over the start line: stop on it and line up, Ready...
            podPointer_ = 1;
            player.vel += -z * (100.0f - zd) - x * ((160.0f - xd) * 0.1f);
            player.doPanel(18, 0);
            player.timerMinutes = kStageMinutes[current_ - 1];
            player.timerState = 0;
            player.scoreReset = true;
            if (current_ == 5) world.traffic.setSpawnLimit(20);
            state_ = 1;
            player.sfx.push_back(26);   // RaceStartFanFair
            lightStartTicks_ = ticks_;
            lightPhase_ = 1;
            lightsU_ = 0.0f;
            arrowsV_ = 0.0f;
            placePods();
            if (current_ == 8) world.plane.startApproach();
            for (size_t i = 0; i < world.nitroPickups.size(); ++i) {
                World::NitroPickup &p = world.nitroPickups[i];
                p.visible = i < 3;
                if (p.visible) p.pos = kStageNitro[i] + Vec3(0, 55, 0);
            }
        }
        break;
    }
    case 1:
        mistV_ += 0.05f;   // translate(0, -0.05, 0)
        if (diff.length() >= 600.0f) break;
        if (!inBox) {
            player.doPanel(22, 200);   // POINT STAGE CANCELLED.
            state_ = 0;
            lightPhase_ = 0;
            hidePods(world);
            break;
        }
        if (zDist < 100 && forwardVel > 0) player.vel += -zAxis * (100.0f - zDist);
        player.yawVelDeg -= 2.0f * xAxis.dot(carZ);
        if (ticks_ - lightStartTicks_ > 60) {   // Set, Go!
            lightStartTicks_ = ticks_;
            ++lightPhase_;
            player.doPanel(17 + lightPhase_, 0);
            if (lightPhase_ >= 3) {
                player.timerState = 1;
                podPointer_ = 1;
                state_ = 2;
                player.doPanel(20, 100);
            }
        }
        break;
    case 2:
        if (podPointer_ > st.podCount) {
            player.timerState = 2;      // time bonus
            lightPhase_ = 0;
            player.doPanel(21, 0);      // STAGE COMPLETE!
            state_ = 3;
            startTicks_ = ticks_;
            hidePods(world);
            player.sfx.push_back(27);
            world.traffic.setSpawnLimit(5);
        } else if (hudTimerState() == -1) {
            player.sfx.push_back(28);   // RaceStopFanFair
            hidePods(world);
            state_ = 6;
            player.doPanel(25, 200);    // TIME UP!
            world.traffic.setSpawnLimit(5);
        }
        break;
    case 3:   // (panel 23 offers to submit the score online; not ported)
        if (ticks_ - startTicks_ > 200) {
            state_ = 4;
            startTicks_ = ticks_;
        }
        break;
    case 4:
        if (ticks_ - startTicks_ > 300) state_ = 0;
        break;
    case 6:
        state_ = 0;
        player.scoreReset = true;
        break;
    case 7:
        if (ticks_ - startTicks_ > 300) {
            state_ = 0;
            player.scoreReset = true;
        }
        break;
    default:
        break;
    }

    // doPointPods: each pod reached is 100 points. Stage 8 ends at the
    // landing plane.
    if (state_ == 2 && podVisible_) {
        if (current_ == 8 && podPointer_ == 6) nextPodAt_ = world.plane.position();
        if (current_ == 8 && podPointer_ == 7) podAt_ = world.plane.position();
        if ((player.pos - podAt_).length() < 200.0f) {
            ++podPointer_;
            player.scoreAdd += 100;
            player.sfx.push_back(29);   // TimeBonusCountUp
            if (podPointer_ < st.podCount) {
                placePods();
            } else if (podPointer_ == st.podCount) {
                Vec3 was = podAt_;
                podAt_ = st.pods[podPointer_ - 1];
                podYaw_ = pointAtYaw(podAt_, was) + PI;
                nextPodVisible_ = false;
            } else {
                podVisible_ = false;
            }
        }
        mistU_ += 0.025f;   // translate(0.025, -0.05, 0)
        mistV_ += 0.05f;
    }

    // The flag pole is solid while the flag is out.
    if (flagShown()) flagPolePush(player, flagPosition());

    player.inStage = state_ > 0;
    // The flag leaves with the stage and comes back 5 s after it ends.
    if (prevState_ != 0 && state_ == 0) flagReadyTicks_ = ticks_ + 300;
    prevState_ = state_;
}

void Stage::draw(Renderer &r, const Camera &cam) const {
    if (gFlagMesh >= 0 && flagShown()) {
        // Shaders 3 (start lights) and 5 (scrolling arrows).
        const StageDef &st = kStages[current_ - 1];
        Mat4 m = Mat4::translation(st.flag) * Mat4::rotationY(deg2rad(st.flagYawDeg));
        drawAssetGroups(r, cam, m, gAssets.meshes[size_t(gFlagMesh)],
                        [&](int g, uint32_t &, float &du, float &dv) {
                            if (g == 2) du = -lightsU_;
                            if (g == 4) dv = -arrowsV_;
                            return true;
                        });
    }
    if (gPodMesh < 0) return;
    const AssetMesh &pod = gAssets.meshes[size_t(gPodMesh)];
    if (podVisible_)
        drawAssetGroups(r, cam, Mat4::translation(podAt_) * Mat4::rotationY(podYaw_), pod,
                        [&](int g, uint32_t &, float &du, float &dv) {
                            if (g == 2) { du = -mistU_; dv = -mistV_; }
                            return true;
                        });
    // mNextPointPod: shaders 1 and 2 diffuse rgb(0, 100, 0) without
    // emission, the mist at blend 0.
    if (nextPodVisible_)
        drawAssetGroups(r, cam, Mat4::translation(nextPodAt_) * Mat4::rotationY(nextPodYaw_), pod,
                        [&](int g, uint32_t &tint, float &, float &) {
                            tint = packRGBA(0, 100, 0);
                            return g != 2;
                        });
}

} // namespace rr4
