#pragma once
// Original Redline Rumble 4 HUD.
//
// The gameplay HUD is not part of the 3D scene: Director behaviours add 2D
// camera overlays on top of the rendered view. This module loads the original
// HUD bitmaps (assets/hud/, extracted by tools/extract_hud.py) and places them
// at the exact overlay coordinates and scales from the decompiled Lingo:
//   BehaviorScript 18 - Score&Timer.ls
//   BehaviorScript 20 - Overlays.ls
//   BehaviorScript 10 - InfoPanels.ls
//   BehaviorScript 9 - MUTE+PAUSE_Button.ls
//   doSetupRadar() in BehaviorScript 4 - PlayerCar.ls
//
// The original stage is 800 x 480; overlay positions below are stage pixels.

#include "renderer.hpp"

#include <string>

namespace rr4 {

struct Car;

struct HudAssets {
    bool loaded = false;

    Texture muteOff, muteOn;
    Texture damageBar, damageRed, skull, redSkull;
    Texture boostBar, boostYellow, boostLogo;
    Texture score[10];
    Texture timer[10];
    Texture timerColon;
    Texture radarRing;
    Texture panel[45];          // indexed by infoPanels member   // InfoPanels messages, tools/render_panels.py
    Texture fadeToBlack;
};

extern HudAssets gHud;

bool loadHudAssets(const std::string &dir = "assets/hud");
// Per-frame overlay logic (Overlays.ls / InfoPanels.ls / FadeToBlack.ls
// exitFrame). Call once per 60 Hz simulation step, after Car::update.
void tickHud(const Car &car);
void drawHud(Renderer &r, const Car &car, bool muted);

} // namespace rr4
