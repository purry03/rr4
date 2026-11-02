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
struct World;

struct HudAssets {
    bool loaded = false;
    std::string dir = "assets/hud";

    Texture muteOff, muteOn;
    Texture damageBar, damageRed, skull, redSkull;
    Texture boostBar, boostYellow, boostLogo;
    Texture score[10];
    Texture timer[10];
    Texture timerColon;
    Texture radarRing, radarFix, radarRace, radarStage, radarPointPod, radarRacePod;
    Texture fadeToBlack;
};

extern HudAssets gHud;

bool loadHudAssets(const std::string &dir = "assets/hud");
// InfoPanels message bitmap (tools/render_panels.py), loaded on first use and
// kept in a small cache: at 3x the 256 x 128 panel a full set would be ~35 MB.
const Texture *hudPanel(int id);
// Per-frame overlay logic (Overlays.ls / InfoPanels.ls / FadeToBlack.ls
// exitFrame). Call once per 60 Hz simulation step, after Car::update.
void tickHud(const Car &car);
// g_TimerState, for gameplay code that waits on the timer (-1 = time up).
int hudTimerState();
// New session: timer, score, panels and fades back to their start values.
void resetHud();
// Drawing in original 800 x 480 stage coordinates (scaled to the renderer).
void drawStageSprite(Renderer &r, const Texture &tex, float x, float y, float scale = 1.0f,
                     float alpha = 1.0f);
void drawStageRect(Renderer &r, float x, float y, float w, float h, uint32_t color, float alpha);
// A texture stretched over a stage rectangle.
void drawStageSpriteRect(Renderer &r, const Texture &tex, float x, float y, float w, float h,
                         float alpha = 1.0f, const SpriteStyle &style = {});
// `cam` and `world` feed doRadar.
void drawHud(Renderer &r, const Car &car, bool muted, const World &world, const Camera &cam);

} // namespace rr4
