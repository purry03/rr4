#include "input.hpp"

#include <SDL.h>

#include <cstdio>

namespace rr4 {

namespace {

// Stick deflection that counts as a d-pad press (of 32767). The steering is
// digital like the original arrow keys, so the stick is a threshold.
constexpr int kStickPress = 16000;
// Hysteresis: the stick must come back under this before it presses again.
constexpr int kStickRelease = 8000;

int stickDirection(int value, int previous) {
    if (value > kStickPress) return 1;
    if (value < -kStickPress) return -1;
    if (previous != 0 && (value > kStickRelease || value < -kStickRelease)) return previous;
    return 0;
}

} // namespace

Input::Input() {
    if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER) && SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
        std::fprintf(stderr, "input: controller init failed (%s)\n", SDL_GetError());
    openPad();
}

Input::~Input() {
    if (pad_) SDL_GameControllerClose(pad_);
}

void Input::openPad() {
    if (pad_) return;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (!SDL_IsGameController(i)) continue;
        pad_ = SDL_GameControllerOpen(i);
        if (pad_) {
            std::printf("input: %s\n", SDL_GameControllerName(pad_));
            return;
        }
    }
}

FrameInput Input::poll(bool inMenu) {
    FrameInput f;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            f.quit = true;
            break;
        case SDL_CONTROLLERDEVICEADDED:
            openPad();
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            if (pad_ && ev.cdevice.which ==
                            SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pad_))) {
                SDL_GameControllerClose(pad_);
                pad_ = nullptr;
                openPad();
            }
            break;
        case SDL_KEYDOWN: {
            if (ev.key.repeat) break;
            const SDL_Keycode key = ev.key.keysym.sym;
            f.toggleMute |= key == SDLK_m;
            f.toggleDebug |= key == SDLK_F1;
            f.screenshot |= key == SDLK_F12;
            if (inMenu) {
                f.menu.up |= key == SDLK_UP || key == SDLK_w;
                f.menu.down |= key == SDLK_DOWN || key == SDLK_s;
                f.menu.left |= key == SDLK_LEFT || key == SDLK_a;
                f.menu.right |= key == SDLK_RIGHT || key == SDLK_d;
                f.menu.confirm |= key == SDLK_RETURN || key == SDLK_KP_ENTER || key == SDLK_SPACE;
                f.menu.back |= key == SDLK_ESCAPE;
            } else {
                f.pause |= key == SDLK_ESCAPE;
            }
            break;
        }
        case SDL_CONTROLLERBUTTONDOWN: {
            const int b = ev.cbutton.button;
            f.toggleMute |= b == SDL_CONTROLLER_BUTTON_BACK;
            if (inMenu) {
                f.menu.up |= b == SDL_CONTROLLER_BUTTON_DPAD_UP;
                f.menu.down |= b == SDL_CONTROLLER_BUTTON_DPAD_DOWN;
                f.menu.left |= b == SDL_CONTROLLER_BUTTON_DPAD_LEFT;
                f.menu.right |= b == SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
                f.menu.confirm |= b == SDL_CONTROLLER_BUTTON_A || b == SDL_CONTROLLER_BUTTON_START;
                f.menu.back |= b == SDL_CONTROLLER_BUTTON_B;
            } else {
                f.pause |= b == SDL_CONTROLLER_BUTTON_START;
            }
            break;
        }
        default:
            break;
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(nullptr);
    f.car.up = keys[SDL_SCANCODE_UP] != 0;
    f.car.down = keys[SDL_SCANCODE_DOWN] != 0;
    f.car.left = keys[SDL_SCANCODE_LEFT] != 0;
    f.car.right = keys[SDL_SCANCODE_RIGHT] != 0;
    f.car.boost = keys[SDL_SCANCODE_LSHIFT] != 0;

    if (pad_) {
        auto held = [&](SDL_GameControllerButton b) {
            return SDL_GameControllerGetButton(pad_, b) != 0;
        };
        const int sx = stickDirection(SDL_GameControllerGetAxis(pad_, SDL_CONTROLLER_AXIS_LEFTX), stickX_);
        const int sy = stickDirection(SDL_GameControllerGetAxis(pad_, SDL_CONTROLLER_AXIS_LEFTY), stickY_);
        f.car.up |= held(SDL_CONTROLLER_BUTTON_A) || held(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) ||
                    held(SDL_CONTROLLER_BUTTON_DPAD_UP);
        f.car.down |= held(SDL_CONTROLLER_BUTTON_X) || held(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) ||
                      held(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        f.car.left |= held(SDL_CONTROLLER_BUTTON_DPAD_LEFT) || sx < 0;
        f.car.right |= held(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || sx > 0;
        f.car.boost |= held(SDL_CONTROLLER_BUTTON_B);
        if (inMenu) {
            f.menu.left |= sx < 0 && stickX_ >= 0;
            f.menu.right |= sx > 0 && stickX_ <= 0;
            f.menu.up |= sy < 0 && stickY_ >= 0;
            f.menu.down |= sy > 0 && stickY_ <= 0;
        }
        stickX_ = sx;
        stickY_ = sy;
    }
    return f;
}

} // namespace rr4
