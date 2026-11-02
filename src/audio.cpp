#include "audio.hpp"
#include "car.hpp"

#include <SDL.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace rr4 {

namespace {

// The original Sfx cast is uncompressed 16-bit mono 44.1 kHz PCM. Clips stay
// mono in memory: that is half the RAM of the stereo copy on the Vita/PSP and
// the mixer only has to duplicate each sample to both output channels.
constexpr int kSampleRate = 44100;
constexpr int kOutChannels = 2;

struct Clip {
    std::vector<int16_t> pcm;   // mono
    size_t frames = 0;

    bool usable() const { return frames > 0; }
    float duration() const { return float(frames) / float(kSampleRate); }
};

// One independent voice with its own play position, like the original port's
// per-member sound objects.
struct Voice {
    const Clip *clip = nullptr;
    size_t pos = 0;
    bool active = false;
    bool loop = false;
    bool paused = false;
    bool music = false;

    void stop() {
        clip = nullptr;
        pos = 0;
        active = false;
        paused = false;
    }
    bool playing() const { return active && !paused && clip && clip->usable(); }
    float offsetSeconds() const { return float(pos) / float(kSampleRate); }
};

std::string soundPath(const std::string &dir, const char *name) {
    return dir + "/" + name + ".wav";
}

int16_t clamp16(int32_t v) {
    return int16_t(std::clamp(v, -32768, 32767));
}

} // namespace

struct Audio::Impl {
    SDL_AudioDeviceID device = 0;
    bool ownsAudioSubsystem = false;
    bool loaded = false;
    bool muted = false;
    float musicGain = 1.0f, sfxGain = 1.0f;

    Voice music, engine, trans, skid, oneShot;
    std::array<Voice *, 5> voices() {
        return {&music, &engine, &trans, &skid, &oneShot};
    }

    // Member storage, matching the original Sfx cast names.
    Clip musicClip;
    std::array<Clip, 6> engineUp, engineDown;
    const Clip *engineUpPtr = nullptr;
    const Clip *engineDownPtr = nullptr;
    Clip reverse, brake;
    Clip skidLeft, skidRight;
    Clip suspension, landing, nitro, repairs, pickup, ferris;
    Clip raceStart, raceStop, raceWin, podBonus;
    std::array<Clip, 4> crash;   // small, standard, medium, big

    const Clip *lastOneShot = nullptr;   // soundMemberList[1]
    int engineState = 0;                 // 0 idle, 1 revving up, 2 revving down
    float complete = 1.0f;               // soundCompletePercent
    int skidDir = 0;
    bool downWas = false;
    bool boostWas = false;
    int carId = 1;

    // The audio callback runs on SDL's audio thread; state changes on the game
    // thread take the device lock so a voice is never half-updated.
    void lock() { if (device) SDL_LockAudioDevice(device); }
    void unlock() { if (device) SDL_UnlockAudioDevice(device); }

    static void mix(void *userdata, Uint8 *stream, int len);

    bool loadClip(const std::string &path, Clip &out);
    void play(Voice &v, const Clip &clip, float offsetSeconds = 0.0f, bool loop = false);
    void playOneShot(const Clip &clip);
    void playEngine(const Clip &clip, float offsetSeconds);
    void stopVoice(Voice &v);

    bool load(const std::string &dir);
    void setCar(int id);
    void startMusic();
    void setMuted(bool m);
    void setVolumes(float musicVolume, float sfxVolume);
    void stopAll();
    void tickEngine(bool accelerate);
    void tickCar(const Car &car, const CarInput &in);
    void impact(float magnitude);
};

void Audio::Impl::mix(void *userdata, Uint8 *stream, int len) {
    auto *self = static_cast<Impl *>(userdata);
    std::memset(stream, 0, size_t(len));
    auto *out = reinterpret_cast<int16_t *>(stream);
    const size_t frameCount = size_t(len) / (sizeof(int16_t) * kOutChannels);

    for (Voice *v : self->voices()) {
        if (!v->active || v->paused || !v->clip || !v->clip->usable()) continue;
        const Clip &clip = *v->clip;
        const float gain = v->music ? self->musicGain : self->sfxGain;
        size_t p = v->pos;
        size_t i = 0;
        for (; i < frameCount; ++i) {
            if (p >= clip.frames) {
                if (v->loop) {
                    p = 0;
                } else {
                    v->active = false;
                    v->clip = nullptr;
                    break;
                }
            }
            const int16_t sample = clamp16(int32_t(float(clip.pcm[p]) * gain));
            out[2 * i] = clamp16(int32_t(out[2 * i]) + int32_t(sample));
            out[2 * i + 1] = clamp16(int32_t(out[2 * i + 1]) + int32_t(sample));
            ++p;
        }
        if (v->active) {
            v->pos = v->loop && clip.frames > 0 ? p % clip.frames : p;
            if (v->pos >= clip.frames) {
                v->active = false;
                v->clip = nullptr;
            }
        }
    }
}

bool Audio::Impl::loadClip(const std::string &path, Clip &out) {
    SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "rb");
    if (!rw) {
        std::fprintf(stderr, "audio: missing %s\n", path.c_str());
        return false;
    }
    SDL_AudioSpec spec{};
    Uint8 *buf = nullptr;
    Uint32 length = 0;
    if (!SDL_LoadWAV_RW(rw, 1, &spec, &buf, &length)) {
        std::fprintf(stderr, "audio: cannot decode %s (%s)\n", path.c_str(), SDL_GetError());
        return false;
    }

    SDL_AudioStream *stream = SDL_NewAudioStream(
        spec.format, spec.channels, spec.freq, AUDIO_S16SYS, 1, kSampleRate);
    if (!stream) {
        std::fprintf(stderr, "audio: cannot convert %s (%s)\n", path.c_str(), SDL_GetError());
        SDL_FreeWAV(buf);
        return false;
    }
    if (SDL_AudioStreamPut(stream, buf, int(length)) < 0) {
        std::fprintf(stderr, "audio: cannot convert %s (%s)\n", path.c_str(), SDL_GetError());
        SDL_FreeAudioStream(stream);
        SDL_FreeWAV(buf);
        return false;
    }
    SDL_AudioStreamFlush(stream);

    int available = SDL_AudioStreamAvailable(stream);
    out.pcm.clear();
    while (available > 0) {
        const size_t old = out.pcm.size();
        out.pcm.resize(old + size_t(available) / sizeof(int16_t));
        const int got = SDL_AudioStreamGet(stream, out.pcm.data() + old, available);
        if (got <= 0) break;
        out.pcm.resize(old + size_t(got) / sizeof(int16_t));
        available = SDL_AudioStreamAvailable(stream);
    }
    out.frames = out.pcm.size();
    SDL_FreeAudioStream(stream);
    SDL_FreeWAV(buf);
    return out.usable();
}

void Audio::Impl::stopVoice(Voice &v) {
    lock();
    v.stop();
    unlock();
}

void Audio::Impl::play(Voice &v, const Clip &clip, float offsetSeconds, bool loop) {
    if (!loaded || muted || !clip.usable()) return;
    lock();
    size_t start = 0;
    if (offsetSeconds > 0.0f && offsetSeconds < clip.duration())
        start = std::min(size_t(offsetSeconds * float(kSampleRate)), clip.frames - 1);
    v.clip = &clip;
    v.pos = start;
    v.active = true;
    v.loop = loop;
    v.paused = false;
    unlock();
}

void Audio::Impl::playEngine(const Clip &clip, float offsetSeconds) {
    play(engine, clip, offsetSeconds, false);
}

void Audio::Impl::playOneShot(const Clip &clip) {
    if (!loaded || muted || !clip.usable()) return;
    lock();
    oneShot.clip = &clip;
    oneShot.pos = 0;
    oneShot.active = true;
    oneShot.loop = false;
    oneShot.paused = false;
    lastOneShot = &clip;
    unlock();
}

bool Audio::Impl::load(const std::string &dir) {
    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            std::fprintf(stderr, "audio: SDL audio init failed (%s)\n", SDL_GetError());
            return false;
        }
        ownsAudioSubsystem = true;
    }

    SDL_AudioSpec want{};
    want.freq = kSampleRate;
    want.format = AUDIO_S16SYS;
    want.channels = kOutChannels;
    want.samples = 1024;
    want.callback = &Impl::mix;
    want.userdata = this;
    SDL_AudioSpec have{};
    device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (!device) {
        std::fprintf(stderr, "audio: cannot open device (%s)\n", SDL_GetError());
        return false;
    }
    if (have.freq != kSampleRate || have.format != AUDIO_S16SYS ||
        have.channels != kOutChannels) {
        std::fprintf(stderr, "audio: device format mismatch (%d Hz, %d ch)\n",
                     have.freq, have.channels);
    }
    SDL_PauseAudioDevice(device, 0);

    auto loadBuf = [&](Clip &clip, const char *name) {
        return loadClip(soundPath(dir, name), clip);
    };

    bool ok = true;
    // One engine pair per original car (Sfx members playerCarID*2-1 / *2).
    for (int i = 0; i < carCount(); ++i) {
        const CarConfig &cfg = carConfig(i + 1);
        ok &= loadBuf(engineUp[size_t(i)], cfg.engineUp);
        ok &= loadBuf(engineDown[size_t(i)], cfg.engineDown);
    }
    setCar(1);
    // doControls: member 13 reversing, member 14 braking.
    ok &= loadBuf(reverse, "Bitsasushi_Reversing_000");
    ok &= loadBuf(brake, "Bitsasushi_Braking_000");
    // doTireGrip: member 42 left, member 43 right.
    ok &= loadBuf(skidLeft, "Bitsasushi_SkiddingLeft_000");
    ok &= loadBuf(skidRight, "Bitsasushi_SkiddingRight_000");
    // Channel 1 and 3 one-shots.
    ok &= loadBuf(suspension, "SuspensionCreak_000");
    ok &= loadBuf(landing, "CrashLanding_001");
    ok &= loadBuf(nitro, "igNitroIgnition_003");
    ok &= loadBuf(repairs, "RixRapidRepairs_000");   // member 24
    ok &= loadBuf(pickup, "SfxGoldPickup_001");      // member 25
    ok &= loadBuf(ferris, "FerrisRotation_001");     // member 22
    ok &= loadBuf(raceStart, "RaceStartFanFair_000"); // member 26
    ok &= loadBuf(raceStop, "RaceStopFanFair_001");   // member 28
    ok &= loadBuf(raceWin, "RaceWinFanFair_000");     // member 27
    ok &= loadBuf(podBonus, "TimeBonusCountUp_001");  // member 29
    ok &= loadBuf(crash[0], "CrashTinkle_Small_000");
    ok &= loadBuf(crash[1], "CrashTinkle_Standard_000");
    ok &= loadBuf(crash[2], "CrashTinkle_Medium_000");
    ok &= loadBuf(crash[3], "CrashTinkle_Big_000");

    // Channel 5, started by doSetupPlayerCar with member 15.
    if (!loadBuf(musicClip, "ChemicalBro_loop_001")) ok = false;

    music.music = true;   // selects the music gain in the mixer
    setVolumes(musicGain * 100.0f, sfxGain * 100.0f);

    loaded = ok;
    std::printf("audio: original Sfx cast loaded (%s)\n", ok ? "ok" : "partial");
    return ok;
}

void Audio::Impl::setCar(int id) {
    const int i = std::clamp(id, 1, carCount()) - 1;
    engineUpPtr = &engineUp[size_t(i)];
    engineDownPtr = &engineDown[size_t(i)];
    carId = i + 1;
    stopVoice(engine);
    engineState = 0;
    complete = 1.0f;
}

void Audio::Impl::startMusic() {
    if (!loaded || muted) return;
    play(music, musicClip, 0.0f, true);
}

void Audio::Impl::stopAll() {
    for (Voice *v : {&engine, &trans, &skid, &oneShot}) stopVoice(*v);
    skidDir = 0;
    engineState = 0;
    complete = 1.0f;
    downWas = false;
    boostWas = false;
}

void Audio::Impl::setMuted(bool m) {
    if (muted == m) return;
    muted = m;
    if (muted) {
        // MUTE+PAUSE_Button.ls: puppetSound(1..4, 0), sound(5).pause().
        lock();
        music.paused = true;
        unlock();
        stopAll();
    } else {
        lock();
        music.paused = false;
        unlock();
        if (!loaded) return;
        // Restart the engine state machine when the next frame is stepped.
        engineState = 0;
        complete = 1.0f;
    }
}

void Audio::Impl::setVolumes(float musicVolume, float sfxVolume) {
    musicGain = std::clamp(musicVolume, 0.0f, 100.0f) / 100.0f;
    sfxGain = std::clamp(sfxVolume, 0.0f, 100.0f) / 100.0f;
}

void Audio::Impl::tickEngine(bool accelerate) {
    if (!loaded || muted) return;
    if (!engineUpPtr || !engineDownPtr) return;
    const float upDuration = engineUpPtr->duration();
    const float downDuration = engineDownPtr->duration();
    if (upDuration <= 0.0f || downDuration <= 0.0f) return;

    const bool busy = engine.playing();

    switch (engineState) {
    case 0:
        if (accelerate) {
            engineState = 1;
            const float start = upDuration * (1.0f - complete);
            playEngine(*engineUpPtr, start < upDuration ? start : 0.0f);
        }
        break;
    case 1:
        complete = busy ? engine.offsetSeconds() / upDuration : 1.0f;
        if (!busy) {
            // doEngineSounds replays the rev-up member when the channel frees.
            playEngine(*engineUpPtr, 0.0f);
        }
        if (!accelerate) {
            engineState = 2;
            const float start = downDuration * (1.0f - complete);
            playEngine(*engineDownPtr, start < downDuration ? start : 0.0f);
        }
        break;
    case 2:
        complete = busy ? engine.offsetSeconds() / downDuration : 1.0f;
        if (complete >= 1.0f || !busy) {
            complete = 1.0f;
            engineState = 0;
        }
        if (accelerate) {
            engineState = 1;
            const float start = upDuration * (1.0f - complete);
            playEngine(*engineUpPtr, start < upDuration ? start : 0.0f);
        }
        break;
    default:
        break;
    }
}

void Audio::Impl::tickCar(const Car &car, const CarInput &in) {
    if (!loaded || muted) return;
    // The Ferris wheel can swap the car: follow it to its engine Sfx pair.
    if (car.carId != carId) setCar(car.carId);
    // doFerrisWheel: Sfx 21 when parking if channel 1 is free, Sfx 22 on
    // channel 2 (the engine channel) each time the wheel turns.
    if (car.ferrisCreakSfx && !oneShot.playing()) playOneShot(suspension);
    if (car.ferrisTurnSfx) {
        stopVoice(engine);
        play(trans, ferris, 0.0f, false);
    }

    // doControls / doControls landing checks (channel 1).
    if (car.landImpact > 20.0f) {
        playOneShot(landing);
    } else if (car.landImpact > 10.0f) {
        playOneShot(suspension);
    }

    // Channel 2 is shared by the engine and the brake/reverse one-shots. The
    // original overrides the engine while Down is held.
    const bool down = in.down && !in.up;
    if (down) {
        stopVoice(engine);
        engineState = 0;
        complete = 1.0f;
        if (!downWas || !trans.playing()) {
            // rollVel > 0 selects braking, otherwise reversing.
            if (car.forwardSpeed() > 0.0f)
                play(trans, brake, 0.0f, false);
            else
                play(trans, reverse, 0.0f, false);
        }
    }
    downWas = down;

    // doTireGrip (channel 4): member 43 is the right skid, 42 the left.
    const float lateral = car.lateralSpeed();
    int dir = 0;
    if (!car.midAir) {
        if (lateral > 8.0f) dir = 1;
        else if (lateral < -8.0f) dir = -1;
    }
    if (dir != 0) {
        if (dir != skidDir || !skid.playing()) {
            if (dir > 0) play(skid, skidRight, 0.0f, false);
            else play(skid, skidLeft, 0.0f, false);
        }
    } else {
        stopVoice(skid);
    }
    skidDir = dir;

    // doAnims (channel 3): ignition fires once when boost starts.
    const bool boosting = in.boost && car.boostMeter > 0.0f && !car.parked;
    if (boosting && !boostWas) playOneShot(nitro);
    boostWas = boosting;

    if (car.impactSpeed > 0.0f) impact(car.impactSpeed);
    // doDamageMaxOutReset plays member 24 on channel 1.
    if (car.respawned) playOneShot(repairs);
    // Race handlers: puppetSound(1, member(n, "Sfx")).
    for (int member : car.sfx) {
        if (member == 26) playOneShot(raceStart);
        else if (member == 27) playOneShot(raceWin);
        else if (member == 28) playOneShot(raceStop);
        else if (member == 29) playOneShot(podBonus);
        // doCollisions traffic bumps: CrashTinkle small / medium.
        else if (member == 16) playOneShot(crash[0]);
        else if (member == 18) playOneShot(crash[2]);
    }
    // doIgNitro: member 25 on channel 1 for every bottle.
    if (car.nitroPickedUp) playOneShot(pickup);
    // doRixRapidRepairs replays member 24 only when channel 1 last played
    // something else.
    if (car.repairSfx && lastOneShot != &repairs) playOneShot(repairs);
}

void Audio::Impl::impact(float magnitude) {
    if (!loaded || muted) return;
    int level = -1;
    if (magnitude >= 20.0f) level = 3;
    else if (magnitude >= 15.0f) level = 2;
    else if (magnitude >= 10.0f) level = 1;
    else if (magnitude >= 5.0f) level = 0;
    if (level >= 0) playOneShot(crash[size_t(level)]);
}

Audio::Audio() : impl_(std::make_unique<Impl>()) {}

Audio::~Audio() {
    if (impl_->device) {
        SDL_PauseAudioDevice(impl_->device, 1);
        SDL_CloseAudioDevice(impl_->device);
    }
    if (impl_->ownsAudioSubsystem) SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool Audio::load(const std::string &dir) { return impl_->load(dir); }
void Audio::startMusic() { impl_->startMusic(); }
void Audio::setCar(int id) { impl_->setCar(id); }
void Audio::toggleMute() { setMuted(!impl_->muted); }
void Audio::setMuted(bool muted) { impl_->setMuted(muted); }
void Audio::setVolumes(float music, float sfx) { impl_->setVolumes(music, sfx); }
void Audio::stopAll() { impl_->stopAll(); }
bool Audio::muted() const { return impl_->muted; }
void Audio::tickEngine(bool accelerate) { impl_->tickEngine(accelerate); }
void Audio::tickCar(const Car &car, const CarInput &in) { impl_->tickCar(car, in); }
void Audio::impact(float magnitude) { impl_->impact(magnitude); }

} // namespace rr4
