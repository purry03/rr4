#include "audio.hpp"
#include "car.hpp"

#include <algorithm>
#include <cstdio>

namespace rr4 {

namespace {

std::string soundPath(const std::string &dir, const char *name) {
    return dir + "/" + name + ".wav";
}

bool usable(const sf::SoundBuffer &b) { return b.getSampleCount() > 0; }

} // namespace

bool Audio::load(const std::string &dir) {
    auto loadBuf = [&](sf::SoundBuffer &b, const char *name) {
        if (!b.loadFromFile(soundPath(dir, name))) {
            std::fprintf(stderr, "audio: missing %s\n", soundPath(dir, name).c_str());
            return false;
        }
        return true;
    };

    bool ok = true;
    // One engine pair per original car (Sfx members playerCarID*2-1 / *2).
    for (int i = 0; i < carCount(); ++i) {
        const CarConfig &cfg = carConfig(i + 1);
        ok &= loadBuf(engineUp_[size_t(i)], cfg.engineUp);
        ok &= loadBuf(engineDown_[size_t(i)], cfg.engineDown);
    }
    setCar(1);
    // doControls: member 13 reversing, member 14 braking.
    ok &= loadBuf(reverse_, "Bitsasushi_Reversing_000");
    ok &= loadBuf(brake_, "Bitsasushi_Braking_000");
    // doTireGrip: member 42 left, member 43 right.
    ok &= loadBuf(skidLeft_, "Bitsasushi_SkiddingLeft_000");
    ok &= loadBuf(skidRight_, "Bitsasushi_SkiddingRight_000");
    // Channel 1 and 3 one-shots.
    ok &= loadBuf(suspension_, "SuspensionCreak_000");
    ok &= loadBuf(landing_, "CrashLanding_001");
    ok &= loadBuf(nitro_, "igNitroIgnition_003");
    ok &= loadBuf(repairs_, "RixRapidRepairs_000");   // member 24
    ok &= loadBuf(pickup_, "SfxGoldPickup_001");      // member 25
    ok &= loadBuf(ferris_, "FerrisRotation_001");     // member 22
    ok &= loadBuf(raceStart_, "RaceStartFanFair_000"); // member 26
    ok &= loadBuf(raceStop_, "RaceStopFanFair_001");   // member 28
    ok &= loadBuf(podBonus_, "TimeBonusCountUp_001");  // member 29
    ok &= loadBuf(crash_[0], "CrashTinkle_Small_000");
    ok &= loadBuf(crash_[1], "CrashTinkle_Standard_000");
    ok &= loadBuf(crash_[2], "CrashTinkle_Medium_000");
    ok &= loadBuf(crash_[3], "CrashTinkle_Big_000");

    // Channel 5, started by doSetupPlayerCar with member 15.
    if (!music_.openFromFile(soundPath(dir, "ChemicalBro_loop_001"))) {
        std::fprintf(stderr, "audio: missing %s\n",
                     soundPath(dir, "ChemicalBro_loop_001").c_str());
        ok = false;
    } else {
        music_.setLooping(true);
        music_.setVolume(100.0f);
    }

    engine_.setVolume(100.0f);
    trans_.setVolume(100.0f);
    skid_.setVolume(100.0f);
    oneShot_.setVolume(100.0f);

    loaded_ = ok;
    std::printf("audio: original Sfx cast loaded (%s)\n", ok ? "ok" : "partial");
    return ok;
}

void Audio::setCar(int id) {
    int i = std::clamp(id, 1, carCount()) - 1;
    engineUpPtr_ = &engineUp_[size_t(i)];
    engineDownPtr_ = &engineDown_[size_t(i)];
    carId_ = i + 1;
    engine_.stop();
    engineState_ = 0;
    complete_ = 1.0f;
}

void Audio::startMusic() {
    if (!loaded_ || muted_) return;
    music_.play();
}

void Audio::stopAll() {
    engine_.stop();
    trans_.stop();
    skid_.stop();
    oneShot_.stop();
    skidDir_ = 0;
    engineState_ = 0;
    complete_ = 1.0f;
    downWas_ = false;
    boostWas_ = false;
}

void Audio::toggleMute() { setMuted(!muted_); }

void Audio::setMuted(bool muted) {
    if (muted_ == muted) return;
    muted_ = muted;
    if (muted_) {
        // MUTE+PAUSE_Button.ls: puppetSound(1..4, 0), sound(5).pause().
        music_.pause();
        stopAll();
    } else {
        music_.play();
        if (!loaded_) return;
        // Restart the engine state machine when the next frame is stepped.
        engineState_ = 0;
        complete_ = 1.0f;
    }
}

void Audio::playEngine(const sf::SoundBuffer &buffer, float offsetSeconds) {
    if (!loaded_ || muted_ || !usable(buffer)) return;
    float duration = buffer.getDuration().asSeconds();
    if (offsetSeconds <= 0.0f || offsetSeconds >= duration) offsetSeconds = 0.0f;
    engine_.setBuffer(buffer);
    engine_.setLooping(false);
    engine_.setPlayingOffset(sf::seconds(offsetSeconds));
    engine_.play();
}

void Audio::playOneShot(const sf::SoundBuffer &buffer) {
    if (!loaded_ || muted_ || !usable(buffer)) return;
    oneShot_.stop();
    oneShot_.setBuffer(buffer);
    oneShot_.play();
    lastOneShot_ = &buffer;
}

void Audio::tickEngine(bool accelerate) {
    if (!loaded_ || muted_) return;

    if (!engineUpPtr_ || !engineDownPtr_) return;
    const float upDuration = engineUpPtr_->getDuration().asSeconds();
    const float downDuration = engineDownPtr_->getDuration().asSeconds();
    if (upDuration <= 0.0f || downDuration <= 0.0f) return;

    const bool busy = engine_.getStatus() == sf::Sound::Status::Playing;

    switch (engineState_) {
    case 0:
        if (accelerate) {
            engineState_ = 1;
            float start = upDuration * (1.0f - complete_);
            playEngine(*engineUpPtr_, start < upDuration ? start : 0.0f);
        }
        break;
    case 1:
        complete_ = busy ? engine_.getPlayingOffset().asSeconds() / upDuration : 1.0f;
        if (!busy) {
            // doEngineSounds replays the rev-up member when the channel frees.
            playEngine(*engineUpPtr_, 0.0f);
        }
        if (!accelerate) {
            engineState_ = 2;
            float start = downDuration * (1.0f - complete_);
            playEngine(*engineDownPtr_, start < downDuration ? start : 0.0f);
        }
        break;
    case 2:
        complete_ = busy ? engine_.getPlayingOffset().asSeconds() / downDuration : 1.0f;
        if (complete_ >= 1.0f || !busy) {
            complete_ = 1.0f;
            engineState_ = 0;
        }
        if (accelerate) {
            engineState_ = 1;
            float start = upDuration * (1.0f - complete_);
            playEngine(*engineUpPtr_, start < upDuration ? start : 0.0f);
        }
        break;
    default:
        break;
    }
}

void Audio::tickCar(const Car &car, const CarInput &in) {
    if (!loaded_ || muted_) return;
    // The Ferris wheel can swap the car: follow it to its engine Sfx pair.
    if (car.carId != carId_) setCar(car.carId);
    // doFerrisWheel: Sfx 21 when parking if channel 1 is free, Sfx 22 on
    // channel 2 (the engine channel) each time the wheel turns.
    if (car.ferrisCreakSfx && oneShot_.getStatus() != sf::Sound::Status::Playing)
        playOneShot(suspension_);
    if (car.ferrisTurnSfx) {
        engine_.stop();
        trans_.setBuffer(ferris_);
        trans_.play();
    }

    // doControls / doControls landing checks (channel 1).
    if (car.landImpact > 20.0f) {
        playOneShot(landing_);
    } else if (car.landImpact > 10.0f) {
        playOneShot(suspension_);
    }

    // Channel 2 is shared by the engine and the brake/reverse one-shots. The
    // original overrides the engine while Down is held.
    const bool down = in.down && !in.up;
    if (down) {
        engine_.stop();
        engineState_ = 0;
        complete_ = 1.0f;
        if (!downWas_ || trans_.getStatus() != sf::Sound::Status::Playing) {
            // rollVel > 0 selects braking, otherwise reversing.
            if (car.forwardSpeed() > 0.0f)
                trans_.setBuffer(brake_);
            else
                trans_.setBuffer(reverse_);
            trans_.play();
        }
    }
    downWas_ = down;

    // doTireGrip (channel 4): member 43 is the right skid, 42 the left.
    float lateral = car.lateralSpeed();
    int dir = 0;
    if (!car.midAir) {
        if (lateral > 8.0f) dir = 1;
        else if (lateral < -8.0f) dir = -1;
    }
    if (dir != 0) {
        if (dir != skidDir_ || skid_.getStatus() != sf::Sound::Status::Playing) {
            skid_.setBuffer(dir > 0 ? skidRight_ : skidLeft_);
            skid_.play();
        }
    } else {
        skid_.stop();
    }
    skidDir_ = dir;

    // doAnims (channel 3): ignition fires once when boost starts.
    const bool boosting = in.boost && car.boostMeter > 0.0f && !car.midAir;
    if (boosting && !boostWas_) playOneShot(nitro_);
    boostWas_ = boosting;

    if (car.impactSpeed > 0.0f) impact(car.impactSpeed);
    // doDamageMaxOutReset plays member 24 on channel 1.
    if (car.respawned) playOneShot(repairs_);
    // Race handlers: puppetSound(1, member(n, "Sfx")).
    for (int member : car.sfx) {
        if (member == 26) playOneShot(raceStart_);
        else if (member == 28) playOneShot(raceStop_);
        else if (member == 29) playOneShot(podBonus_);
    }
    // doIgNitro: member 25 on channel 1 for every bottle.
    if (car.nitroPickedUp) playOneShot(pickup_);
    // doRixRapidRepairs replays member 24 only when channel 1 last played
    // something else.
    if (car.repairSfx && lastOneShot_ != &repairs_) playOneShot(repairs_);
}

void Audio::impact(float magnitude) {
    if (!loaded_ || muted_) return;
    int level = -1;
    if (magnitude >= 20.0f) level = 3;
    else if (magnitude >= 15.0f) level = 2;
    else if (magnitude >= 10.0f) level = 1;
    else if (magnitude >= 5.0f) level = 0;
    if (level >= 0) playOneShot(crash_[size_t(level)]);
}

} // namespace rr4
