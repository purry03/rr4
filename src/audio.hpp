#pragma once

#include <SFML/Audio.hpp>

#include <array>
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
class Audio {
public:
    bool load(const std::string &dir = "assets/sounds");
    void startMusic();
    void setCar(int id);

    void toggleMute();
    void setMuted(bool muted);
    bool muted() const { return muted_; }

    // doEngineSounds(), called at the start of every 60 Hz step before
    // doControls(), exactly like enterFrame.
    void tickEngine(bool accelerate);

    // Car-driven one-shots after the physics step and world collision pass.
    void tickCar(const Car &car, const CarInput &in);

    // Collision impulse, matching the CrashTinkle thresholds in doCollisions /
    // doRadialPush (5 / 10 / 15 / 20).
    void impact(float magnitude);

private:
    void playEngine(const sf::SoundBuffer &buffer, float offsetSeconds);
    void playOneShot(const sf::SoundBuffer &buffer);
    void stopAll();

    sf::Music music_;
    std::array<sf::SoundBuffer, 6> engineUp_, engineDown_;
    const sf::SoundBuffer *engineUpPtr_ = nullptr;
    const sf::SoundBuffer *engineDownPtr_ = nullptr;
    sf::SoundBuffer reverse_, brake_;
    sf::SoundBuffer skidLeft_, skidRight_;
    sf::SoundBuffer suspension_, landing_, nitro_;
    std::array<sf::SoundBuffer, 4> crash_; // small, standard, medium, big

    sf::Sound engine_{engineUp_[0]};
    sf::Sound trans_{engineDown_[0]};
    sf::Sound skid_{skidLeft_};
    sf::Sound oneShot_{suspension_};

    bool loaded_ = false;
    bool muted_ = false;

    int engineState_ = 0;        // 0 idle, 1 revving up, 2 revving down
    float complete_ = 1.0f;      // soundCompletePercent
    int skidDir_ = 0;
    bool downWas_ = false;
    bool boostWas_ = false;
};

} // namespace rr4
