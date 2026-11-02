#pragma once

#include <memory>
#include <string>

namespace rr4 {

struct Car;
struct CarInput;

// Original Redline Rumble 4 sound playback.
//
// Sound channels in the Director movie (PlayerCar.ls):
//   1  one-shots: CrashTinkle 16..19, CrashLanding 20, SuspensionCreak 21,
//      RixRapidRepairs 24, pickups/race pods 25..29
//   2  engine loops (member playerCarID*2-1 / playerCarID*2), overridden by
//      Bitsasushi_Braking 14 / Bitsasushi_Reversing 13 while Down is held
//   3  igNitroIgnition 23
//   4  Bitsasushi_SkiddingLeft/Right 42/43
//   5  ChemicalBro_loop 15 (music, started by doSetupPlayerCar)
//
// Every car's engine pair is loaded, so Audio::setCar() follows Car::setCar()
// and uses member playerCarID*2-1 / playerCarID*2 exactly like doEngineSounds.
//
// The mixer is a small SDL2 audio callback with five independent voices. The
// Director channel numbers above describe intent, not a hardware limit: the
// original port already played engine/brake/reverse on separate voices.
class Audio {
public:
    Audio();
    ~Audio();
    Audio(const Audio &) = delete;
    Audio &operator=(const Audio &) = delete;

    bool load(const std::string &dir = "assets/sounds");
    void startMusic();
    void setCar(int id);

    void toggleMute();
    void setMuted(bool muted);
    // Settings: music and sound effect volume, 0..100 (percent).
    void setVolumes(float music, float sfx);
    // Silence the car channels (engine, skid, one-shots); music keeps playing.
    void stopAll();
    bool muted() const;

    // doEngineSounds(), called at the start of every 60 Hz step before
    // doControls(), exactly like enterFrame.
    void tickEngine(bool accelerate);

    // Car-driven one-shots after the physics step and world collision pass.
    void tickCar(const Car &car, const CarInput &in);

    // Collision impulse, matching the CrashTinkle thresholds in doCollisions /
    // doRadialPush (5 / 10 / 15 / 20).
    void impact(float magnitude);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    bool muted_ = false;
};

} // namespace rr4
