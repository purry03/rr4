#pragma once

#include <SDL.h>

#include "car.hpp"
#include "menu.hpp"

namespace rr4 {

// SDL game controller support. The original game is keyboard-only (digital
// Up/Down/Left/Right/Shift), so the pad is mapped onto the same digital
// CarInput; analog sticks and triggers are thresholded.
//
//   Left stick X         proportional steering; D-pad steers fully
//   D-pad / left stick   steer, accelerate (up), brake + reverse (down)
//   RT / LT              accelerate / brake + reverse
//   A or X (south/west)  nitro boost (menu: confirm)
//   Right stick X        orbit the camera; R3 recentres (it also returns by itself)
//   B / RB               handbrake (menu: B = back)
//   Start                pause to the menu / menu back
class Gamepad {
public:
    // Stick / trigger value (of 32767) that counts as a press.
    static constexpr Sint16 kStickThreshold = 12000;
    static constexpr Sint16 kTriggerThreshold = 8000;
    // Analog dead zones (fraction of full travel); values are rescaled so
    // the output still spans 0..1 beyond the dead zone.
    // Rumble tuning (not from the original).
    static constexpr float kImpactFullSpeed = 20.0f;  // impactSpeed for full strength (top CrashTinkle step)
    static constexpr int kImpactTicks = 24;           // jolt length, 0.4 s
    static constexpr float kNitroLow = 0.30f;
    static constexpr float kSlideHigh = 0.25f;
    static constexpr float kSlideMinSpeed = 3.0f;     // speed below which a handbrake does not buzz
    static constexpr int kRumbleResendTicks = 4;      // refresh interval
    static constexpr Uint32 kRumbleMs = 100;          // each refresh lasts this long
    static constexpr float kStickDeadZone = 0.15f;
    static constexpr float kTriggerDeadZone = 0.05f;

    void init();
    void shutdown();
    // Feed every SDL event: handles hot-plug and menu edge presses.
    void handleEvent(const SDL_Event &ev);
    // Held state for this frame, to be OR-ed with the keyboard.
    CarInput car() const;
    // Menu navigation presses gathered since the last call (edge triggered,
    // includes stick flicks).
    MenuInput takeMenu();
    // Force feedback for one 60 Hz game step (call after Car::update).
    // Port addition. Impacts (Car::impactSpeed, the same value CrashTinkle
    // uses) give a decaying jolt, igNitro a steady low rumble, a handbrake
    // slide a light high-frequency buzz.
    void rumble(const Car &car, const CarInput &in);
    // Right stick X for the free-look camera, -1..1 with dead zone.
    float cameraAxis() const;
    // True once if R3 (right stick click) was pressed since the last call.
    bool takeCameraReset();
    // True once if Start was pressed since the last call.
    bool takePause();

private:
    void open(int index);
    bool button(SDL_GameControllerButton b) const;
    Sint16 axis(SDL_GameControllerAxis a) const;

    SDL_GameController *pad_ = nullptr;
    SDL_JoystickID padId_ = -1;
    MenuInput pending_;
    bool pause_ = false;
    bool cameraReset_ = false;
    bool rumbleOk_ = false;
    float impactStrength_ = 0.0f;
    int impactLeft_ = 0, rumbleTick_ = 0;
    bool stickYUp_ = false, stickYDown_ = false;
    bool stickLeft_ = false, stickRight_ = false;
};

} // namespace rr4
