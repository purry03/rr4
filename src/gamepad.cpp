#include "gamepad.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace rr4 {

void Gamepad::init() {
    if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER) &&
        SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
        std::fprintf(stderr, "gamepad: init failed (%s)\n", SDL_GetError());
        return;
    }
    for (int i = 0; i < SDL_NumJoysticks() && !pad_; ++i) open(i);
}

void Gamepad::shutdown() {
    if (pad_) SDL_GameControllerClose(pad_);
    pad_ = nullptr;
}

void Gamepad::open(int index) {
    if (pad_ || !SDL_IsGameController(index)) return;
    pad_ = SDL_GameControllerOpen(index);
    if (!pad_) return;
    padId_ = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pad_));
    rumbleOk_ = SDL_GameControllerHasRumble(pad_) == SDL_TRUE;
    std::fprintf(stderr, "gamepad: %s (rumble %s)\n", SDL_GameControllerName(pad_),
                 rumbleOk_ ? "supported" : "NOT supported by this device/driver");
}

bool Gamepad::button(SDL_GameControllerButton b) const {
    return pad_ && SDL_GameControllerGetButton(pad_, b) != 0;
}

Sint16 Gamepad::axis(SDL_GameControllerAxis a) const {
    return pad_ ? SDL_GameControllerGetAxis(pad_, a) : Sint16(0);
}

void Gamepad::handleEvent(const SDL_Event &ev) {
    switch (ev.type) {
    case SDL_CONTROLLERDEVICEADDED:
        open(ev.cdevice.which);
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        if (pad_ && ev.cdevice.which == padId_) {
            shutdown();
            stickYUp_ = stickYDown_ = stickLeft_ = stickRight_ = false;
            for (int i = 0; i < SDL_NumJoysticks() && !pad_; ++i) open(i); // fall back to another pad
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        if (ev.cbutton.which != padId_) break;
        switch (ev.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP: pending_.up = true; break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: pending_.down = true; break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: pending_.left = true; break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: pending_.right = true; break;
        case SDL_CONTROLLER_BUTTON_A: pending_.confirm = true; break;
        case SDL_CONTROLLER_BUTTON_B: pending_.back = true; break;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: cameraReset_ = true; break;
        case SDL_CONTROLLER_BUTTON_START: pause_ = true; pending_.back = true; break;
        default: break;
        }
        break;
    case SDL_CONTROLLERAXISMOTION: {
        if (ev.caxis.which != padId_) break;
        const Sint16 v = ev.caxis.value;
        if (ev.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
            const bool l = v < -kStickThreshold, r = v > kStickThreshold;
            pending_.left |= l && !stickLeft_;
            pending_.right |= r && !stickRight_;
            stickLeft_ = l;
            stickRight_ = r;
        } else if (ev.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
            const bool u = v < -kStickThreshold, d = v > kStickThreshold;
            pending_.up |= u && !stickYUp_;
            pending_.down |= d && !stickYDown_;
            stickYUp_ = u;
            stickYDown_ = d;
        }
        break;
    }
    default: break;
    }
}

namespace {
// Normalise a raw axis to -1..1 with a dead zone.
float shaped(Sint16 raw, float dead) {
    const float v = float(raw) / 32767.0f;
    const float m = std::fabs(v);
    if (m <= dead) return 0.0f;
    return std::copysign(std::min(1.0f, (m - dead) / (1.0f - dead)), v);
}
} // namespace

CarInput Gamepad::car() const {
    CarInput in;
    if (!pad_) return in;
    const float sx = shaped(axis(SDL_CONTROLLER_AXIS_LEFTX), kStickDeadZone);
    const float sy = shaped(axis(SDL_CONTROLLER_AXIS_LEFTY), kStickDeadZone);
    const float rt = shaped(axis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT), kTriggerDeadZone);
    const float lt = shaped(axis(SDL_CONTROLLER_AXIS_TRIGGERLEFT), kTriggerDeadZone);
    // Triggers win over the stick; the D-pad is a full digital press.
    const float pedal = std::clamp(rt - lt != 0.0f ? rt - lt : -sy, -1.0f, 1.0f);
    const bool dl = button(SDL_CONTROLLER_BUTTON_DPAD_LEFT), dr = button(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    const bool du = button(SDL_CONTROLLER_BUTTON_DPAD_UP), dd = button(SDL_CONTROLLER_BUTTON_DPAD_DOWN);

    in.left = dl || sx < 0.0f;
    in.right = dr || sx > 0.0f;
    in.up = du || pedal > 0.0f;
    in.down = dd || pedal < 0.0f;
    // Analog values only when the D-pad is not overriding them.
    if (!dl && !dr) in.steerAxis = sx;
    if (!du && !dd) in.throttleAxis = pedal;
    in.handbrake = button(SDL_CONTROLLER_BUTTON_B) || button(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    in.boost = button(SDL_CONTROLLER_BUTTON_A) || button(SDL_CONTROLLER_BUTTON_X);
    return in;
}

void Gamepad::rumble(const Car &car, const CarInput &in) {
    if (!pad_ || !rumbleOk_) return;
    bool fresh = false;
    if (car.impactSpeed > 0.0f) {
        const float s = std::min(1.0f, car.impactSpeed / kImpactFullSpeed);
        if (impactLeft_ == 0 || s > impactStrength_) { impactStrength_ = s; fresh = true; }
        impactLeft_ = kImpactTicks;
    }
    float low = 0.0f, high = 0.0f;
    if (impactLeft_ > 0) {
        const float fade = float(impactLeft_) / float(kImpactTicks);
        low = impactStrength_ * fade;
        high = impactStrength_ * fade;
        --impactLeft_;
    }
    if (in.boost && car.boostMeter > 0.0f && !car.parked) low = std::max(low, kNitroLow);
    if (in.handbrake && !car.midAir && car.vel.length() > kSlideMinSpeed) high = std::max(high, kSlideHigh);

    if (low <= 0.0f && high <= 0.0f) {
        if (rumbleTick_ > 0) SDL_GameControllerRumble(pad_, 0, 0, 0);
        rumbleTick_ = 0;
        return;
    }
    if (fresh || rumbleTick_ == 0) {
        SDL_GameControllerRumble(pad_, Uint16(low * 65535.0f), Uint16(high * 65535.0f), kRumbleMs);
        rumbleTick_ = kRumbleResendTicks;
    } else if (--rumbleTick_ == 0) {
        rumbleTick_ = 0; // next step resends with the current values
    }
}

MenuInput Gamepad::takeMenu() {
    MenuInput m = pending_;
    pending_ = {};
    return m;
}

float Gamepad::cameraAxis() const {
    return shaped(axis(SDL_CONTROLLER_AXIS_RIGHTX), kStickDeadZone);
}

bool Gamepad::takeCameraReset() {
    const bool r = cameraReset_;
    cameraReset_ = false;
    return r;
}

bool Gamepad::takePause() {
    const bool p = pause_;
    pause_ = false;
    return p;
}

} // namespace rr4
