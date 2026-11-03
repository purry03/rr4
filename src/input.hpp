#pragma once
// Player input for one rendered frame: the keyboard (arrows, Shift, Esc as in
// the original's keyPressed checks) plus the first SDL game controller, which
// is the built-in pad on the PS Vita.
//
// Pad layout: Cross / R / d-pad up accelerate, Square / L / d-pad down brake
// and reverse, Circle igNitro, left stick or d-pad steer, Start pauses,
// Select mutes. In menus: d-pad / stick move, Cross confirms, Circle goes back.

#include "car.hpp"
#include "menu.hpp"

struct _SDL_GameController;

namespace rr4 {

struct FrameInput {
    bool quit = false;
    CarInput car;           // held this frame
    MenuInput menu;         // presses since the last poll
    bool pause = false;     // Esc / Start while driving
    bool toggleMute = false, toggleDebug = false, screenshot = false;
};

class Input {
public:
    Input();
    ~Input();
    Input(const Input &) = delete;
    Input &operator=(const Input &) = delete;

    // Drains the SDL event queue. `inMenu` routes Esc / Circle to MenuInput.
    FrameInput poll(bool inMenu);

private:
    void openPad();
    _SDL_GameController *pad_ = nullptr;
    // Stick direction last frame (-1, 0, 1), for menu presses from the stick.
    int stickX_ = 0, stickY_ = 0;
};

} // namespace rr4
