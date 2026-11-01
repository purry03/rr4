#include "hud.hpp"
#include "car.hpp"
#include "assets.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <string>
#include <cstdio>
#include <vector>

namespace rr4 {

HudAssets gHud;

namespace {

// Original movie stage used by every camera overlay coordinate.
constexpr float kStageW = 800.0f;
constexpr float kStageH = 480.0f;

struct Stage {
    float sx, sy;
    explicit Stage(const Renderer &r)
        : sx(float(r.width) / kStageW), sy(float(r.height) / kStageH) {}

    // addOverlay(texture, loc, 0) uses the bitmap's top-left corner.
    void draw(Renderer &r, const Texture &tex, float x, float y, float scale = 1.0f,
              float alpha = 1.0f) const {
        if (tex.px.empty()) return;
        r.drawSprite(tex, x * sx, y * sy, float(tex.w) * scale * sx,
                     float(tex.h) * scale * sy, alpha);
    }
};

} // namespace

bool loadHudAssets(const std::string &dir) {
    if (gHud.loaded) return true;
    static bool attempted = false;
    if (attempted) return false;
    attempted = true;
    bool ok = true;
    ok &= loadTexture(dir + "/MuteOff.png", gHud.muteOff);
    ok &= loadTexture(dir + "/MuteOn.png", gHud.muteOn);
    ok &= loadTexture(dir + "/DamageBar.png", gHud.damageBar);
    ok &= loadTexture(dir + "/DamageBarRed.png", gHud.damageRed);
    ok &= loadTexture(dir + "/Skull.png", gHud.skull);
    ok &= loadTexture(dir + "/RedSkull.png", gHud.redSkull);
    ok &= loadTexture(dir + "/BoostBar.png", gHud.boostBar);
    ok &= loadTexture(dir + "/BoostBarYellow.png", gHud.boostYellow);
    ok &= loadTexture(dir + "/BoostLogo.png", gHud.boostLogo);
    ok &= loadTexture(dir + "/TimerBackground.png", gHud.timerColon);
    ok &= loadTexture(dir + "/RadarRing.png", gHud.radarRing);
    for (int id : {1, 2, 3, 4, 5, 6, 8, 9, 16, 18, 19, 20, 28, 30, 31, 32, 33, 34, 35, 37,
                   38, 39, 44})
        ok &= loadTexture(dir + "/Panel" + std::to_string(id) + ".png", gHud.panel[id]);
    ok &= loadTexture(dir + "/FadeToBlack.png", gHud.fadeToBlack);
    for (int i = 0; i < 10; ++i) {
        char name[32];
        std::snprintf(name, sizeof(name), "/Score%d.png", i);
        ok &= loadTexture(dir + name, gHud.score[i]);
        std::snprintf(name, sizeof(name), "/Timer%d.png", i);
        ok &= loadTexture(dir + name, gHud.timer[i]);
    }
    gHud.loaded = ok;
    std::printf("hud: original camera overlays %s\n", ok ? "loaded" : "INCOMPLETE");
    return ok;
}

namespace {

// Overlay state advanced by tickHud(). `ticks` counts 60 Hz steps (the ticks).
struct HudState {
    long ticks = 0;
    // Overlays.ls
    float displayDamage = 0.0f, displayBoost = 1.0f;
    float prevDamage = 0.0f;
    int skullBlend = 0;        // value assigned to the red skull this frame
    int skullNext = 0;         // skullBlend after the exitFrame decrement
    // InfoPanels.ls
    int messageID = 0, prevMessageID = 0;
    bool panelUp = false;
    int panelBlend = 0;
    long panelDownTicks = 0;
    // FadeToBlack.ls (g_FadeToBlack = 0 in free drive)
    int fadeBlend = 0;         // value drawn this frame
    int fadeNext = 0;
    // Score&Timer.ls: timeList = [hundredths, tenths, seconds, tens of
    // seconds, minutes] and g_TimerState.
    int time[5] = {};
    int timerState = 0;
};
HudState gState;

// InfoPanels.ls doPanel. Every free-drive call uses rgb(255, 255, 255), which
// tools/render_panels.py bakes into the panel bitmaps.
void doPanel(int messageID, long messageTicks) {
    HudState &h = gState;
    h.panelDownTicks = messageTicks > 0 ? h.ticks + messageTicks : 0;
    if (h.prevMessageID != messageID) {
        h.prevMessageID = messageID;
        if (messageID > 0) h.panelBlend = 0;
    }
    if (messageID > 0) {
        h.messageID = messageID;
        h.panelUp = true;
    } else {
        h.panelUp = false;
    }
}

} // namespace

void tickHud(const Car &car) {
    HudState &h = gState;
    ++h.ticks;

    // doPanel / fadeToBlackBlend / g_TimerState calls made by gameplay code.
    for (const Car::PanelRequest &p : car.panels) doPanel(p.id, p.ticks);
    if (car.fadeToBlack) h.fadeNext = 100;
    if (car.timerReset) for (int &d : h.time) d = 0;
    if (car.timerState != Car::kTimerUnchanged) h.timerState = car.timerState;

    // Score&Timer.ls exitFrame, g_TimerState -2: count up. One tick adds
    // integer(1 * 1.6666667) = 2 hundredths, as in the original.
    if (h.timerState == -2) {
        h.time[0] += 2;
        while (h.time[0] > 9) {
            h.time[0] -= 10; ++h.time[1];
            if (h.time[1] > 9) { h.time[1] -= 10; ++h.time[2]; }
            if (h.time[2] > 9) { h.time[2] -= 10; ++h.time[3]; }
            if (h.time[3] > 5) { h.time[3] -= 6; h.time[4] = std::min(9, h.time[4] + 1); }
        }
    }

    // Overlays.ls exitFrame.
    const float damage = std::min(car.damage, 1.0f);
    h.displayDamage += (damage - h.displayDamage) * 0.2f;
    if (h.prevDamage != damage) h.skullNext = 100;
    h.skullBlend = h.skullNext;
    if (h.skullNext > 0 && damage < 1.0f) h.skullNext -= 5;
    h.prevDamage = damage;
    h.displayBoost += (clampf(car.boostMeter, 0.0f, 1.0f) - h.displayBoost) * 0.2f;

    // InfoPanels.ls exitFrame.
    if (h.panelDownTicks > 0 && h.ticks >= h.panelDownTicks) h.panelUp = false;
    if (h.panelUp) {
        if (h.panelBlend < 70) h.panelBlend += 5;
    } else if (h.panelBlend > 0) {
        h.panelBlend -= 5;
    }

    // FadeToBlack.ls exitFrame: blend is assigned, then decremented by 2.
    h.fadeBlend = h.fadeNext;
    if (h.fadeNext > 0) h.fadeNext -= 2;
}

void drawHud(Renderer &r, const Car &car, bool muted) {
    if (!gHud.loaded && !loadHudAssets()) return;
    const HudState &h = gState;
    Stage s(r);

    // Score&Timer.ls: score is five 32x32 digits at x = 4 + 26*i, y = 28.
    // Free drive has g_CurrentScore = 0.
    int score = 0;
    int scoreDigits[5] = {
        (score / 10000) % 10, (score / 1000) % 10, (score / 100) % 10,
        (score / 10) % 10, score % 10,
    };
    for (int i = 0; i < 5; ++i)
        s.draw(r, gHud.score[scoreDigits[i]], 4.0f + 26.0f * float(i), 28.0f);

    // Timer: minutes at 640, colon at 664, tens/ones at 692/718, then
    // tenths/hundredths at 756/772 with scale 0.5. Races run the timer.
    const int timer[5] = {h.time[4], h.time[3], h.time[2], h.time[1], h.time[0]};
    s.draw(r, gHud.timer[timer[0]], 640.0f, 28.0f);
    s.draw(r, gHud.timerColon, 664.0f, 28.0f);
    s.draw(r, gHud.timer[timer[1]], 692.0f, 28.0f);
    s.draw(r, gHud.timer[timer[2]], 718.0f, 28.0f);
    s.draw(r, gHud.timer[timer[3]], 756.0f, 30.0f, 0.5f);
    s.draw(r, gHud.timer[timer[4]], 772.0f, 30.0f, 0.5f);

    // Overlays.ls: damage bar at (0,8), red fill at -254 + 254 * damage
    // (displayDamagePercent eases 20% per frame). The red skull at (246,1)
    // lights to full blend whenever the damage changes and fades 5 per frame;
    // at 100 % damage it stays lit.
    s.draw(r, gHud.damageBar, 0.0f, 8.0f);
    s.draw(r, gHud.damageRed, -254.0f + h.displayDamage * 254.0f, 8.0f);
    s.draw(r, gHud.skull, 214.0f, 0.0f);
    if (h.skullBlend > 0) s.draw(r, gHud.redSkull, 246.0f, 1.0f, 1.0f, float(h.skullBlend) / 100.0f);

    // boost fill anchor: 800 - 250 * g_BoostPercent.
    float boost = h.displayBoost;
    s.draw(r, gHud.boostBar, 544.0f, 8.0f);
    s.draw(r, gHud.boostYellow, 800.0f - boost * 250.0f, 8.0f);
    s.draw(r, gHud.boostLogo, 505.0f, 2.0f);

    // MUTE+PAUSE_Button.ls: speaker icon at (776,460).
    s.draw(r, muted ? gHud.muteOn : gHud.muteOff, 776.0f, 460.0f);

    // doSetupRadar(): radar ring at (20,400). The pod icons only appear when
    // the race/stage systems place them; free drive leaves them hidden.
    s.draw(r, gHud.radarRing, 20.0f, 400.0f);

    // InfoPanels.ls: panel overlay at (278,9), scale panelBlend / 70 from its
    // top-left registration point. Rising it is fully opaque; falling, the
    // blend follows panelBlend.
    if (h.panelBlend > 0) {
        const Texture &panel = gHud.panel[h.messageID];
        float scale = float(h.panelBlend) / 70.0f;
        float alpha = h.panelUp ? 1.0f : float(h.panelBlend) / 100.0f;
        s.draw(r, panel, 278.0f, 9.0f, scale, alpha);
    }

    // FadeToBlack.ls: member 52 at (0,0), scale 100, blend fadeToBlackBlend.
    if (h.fadeBlend > 0) s.draw(r, gHud.fadeToBlack, 0.0f, 0.0f, 100.0f, float(h.fadeBlend) / 100.0f);
}

} // namespace rr4
