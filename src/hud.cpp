#include "hud.hpp"
#include "world.hpp"
#include "car.hpp"
#include "assets.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <string>
#include <cstdio>
#include <vector>

namespace rr4 {

HudAssets gHud;

namespace {

// Original movie stage used by every camera overlay coordinate.
constexpr float kStageW = 800.0f;
constexpr float kStageH = 480.0f;

struct StageScreen {
    float sx, sy;
    explicit StageScreen(const Renderer &r)
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
    gHud.dir = dir;
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
    ok &= loadTexture(dir + "/RadarFix.png", gHud.radarFix);
    ok &= loadTexture(dir + "/RadarRace.png", gHud.radarRace);
    ok &= loadTexture(dir + "/RadarStage.png", gHud.radarStage);
    ok &= loadTexture(dir + "/RadarPointPod.png", gHud.radarPointPod);
    ok &= loadTexture(dir + "/RadarRacePod.png", gHud.radarRacePod);
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
// tools/render_panels.py bakes the InfoPanels messages at this multiple of
// their 256 x 128 stage size; the HUD draws them at 1 / kPanelBakeScale.
constexpr float kPanelBakeScale = 3.0f;
} // namespace

const Texture *hudPanel(int id) {
    if (id <= 0 || id >= 45) return nullptr;
    struct Slot {
        int id = -1;
        Texture tex;
    };
    static Slot slots[4];   // recent messages, in display order
    static int next = 0;
    for (Slot &s : slots)
        if (s.id == id && !s.tex.px.empty()) return &s.tex;
    Slot &s = slots[size_t(next++) % std::size(slots)];
    s.id = -1;
    if (!loadTexture(gHud.dir + "/Panel" + std::to_string(id) + ".png", s.tex)) return nullptr;
    s.tex.buildMips();
    s.id = id;
    return &s.tex;
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
    int score = 0;             // g_CurrentScore
    long waitTicks = 0;
};
HudState gState;

// timeList borrow after a deduction (Score&Timer.ls). Returns true when the
// seconds and minutes ran out.
bool borrowTime(int *t) {
    while (t[0] < 0) {
        t[0] += 10; --t[1];
        if (t[1] < 0) { t[1] += 10; --t[2]; }
        if (t[2] < 0) { t[2] += 10; --t[3]; }
        if (t[3] < 0) { t[3] += 6; if (t[4] > 0) --t[4]; }
        if (t[2] == 0 && t[3] == 0 && t[4] == 0) return true;
    }
    return false;
}

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

int hudTimerState() { return gState.timerState; }

void resetHud() { gState = HudState{}; }

void drawStageSprite(Renderer &r, const Texture &tex, float x, float y, float scale, float alpha) {
    StageScreen(r).draw(r, tex, x, y, scale, alpha);
}

void drawStageSpriteRect(Renderer &r, const Texture &tex, float x, float y, float w, float h,
                         float alpha, const SpriteStyle &style) {
    const StageScreen s(r);
    r.drawSprite(tex, x * s.sx, y * s.sy, w * s.sx, h * s.sy, alpha, style);
}

void drawStageRect(Renderer &r, float x, float y, float w, float h, uint32_t color, float alpha) {
    const StageScreen s(r);
    const int x0 = int(std::lround(x * s.sx)), y0 = int(std::lround(y * s.sy));
    r.drawRect(x0, y0, int(std::lround((x + w) * s.sx)) - x0, int(std::lround((y + h) * s.sy)) - y0,
               color, alpha);
}

void tickHud(const Car &car) {
    HudState &h = gState;
    ++h.ticks;

    // doPanel / fadeToBlackBlend / g_TimerState calls made by gameplay code.
    for (const Car::PanelRequest &p : car.panels) doPanel(p.id, p.ticks);
    if (car.fadeToBlack) h.fadeNext = 100;
    if (car.timerReset) for (int &d : h.time) d = 0;
    if (car.timerMinutes >= 0) {
        for (int &d : h.time) d = 0;
        h.time[4] = car.timerMinutes;
    }
    if (car.timerState != Car::kTimerUnchanged) h.timerState = car.timerState;
    if (car.scoreReset) h.score = 0;
    h.score = std::min(99999, h.score + car.scoreAdd);

    // g_TimerState 1: count down; at 0:00 the time is up (-1).
    if (h.timerState == 1) {
        h.time[0] -= 2;
        if (borrowTime(h.time)) {
            for (int &d : h.time) d = 0;
            h.timerState = -1;
        }
    }
    // g_TimerState 2: stage complete, the time left drains into the score
    // (+23 per 1.23 s), then 450 ticks on show. (The original also zooms the
    // score digits to the middle of the screen and back.)
    if (h.timerState == 2) {
        h.score = std::min(99999, h.score + 23);
        h.time[0] -= 123;
        if (borrowTime(h.time)) {
            h.score = std::min(99999, h.score + h.time[0] + h.time[1] * 10);
            for (int &d : h.time) d = 0;
            h.timerState = 3;
            h.waitTicks = h.ticks;
        }
    } else if (h.timerState == 3 && h.ticks - h.waitTicks > 450) {
        h.timerState = 0;
    }

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

void drawHud(Renderer &r, const Car &car, bool muted, const World &world, const Camera &cam) {
    if (!gHud.loaded && !loadHudAssets()) return;
    const HudState &h = gState;
    StageScreen s(r);

    // Score&Timer.ls: score is five 32x32 digits at x = 4 + 26*i, y = 28.
    int score = h.score;
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

    // doSetupRadar(): the ring at (20,400), then the blips. doRadar puts each
    // blip along the target's direction in the camera's ground plane (up =
    // ahead), `scale` stage pixels per world unit, at most 32 from `centre`.
    s.draw(r, gHud.radarRing, 20.0f, 400.0f);
    const Vec3 front = (cam.target - cam.pos).normalized();      // -zAxis
    const Vec3 side = front.cross(cam.up).normalized() * -1.0f;  // -xAxis
    auto blip = [&](const Texture &tex, const Vec3 &target, float scale, float cx, float cy) {
        Vec3 to = cam.pos - target;
        Vec3 v(side.dot(to), 0.0f, front.dot(to));
        float mag = std::min(v.length() * scale, 32.0f);
        if (v.length() > 1e-4f) v = v.normalized();
        s.draw(r, tex, cx + v.x * mag, cy + v.z * mag);
    };
    if (world.objectsReady) {
        // Rix's garage, the next race flag and the points flag show while
        // no pod is out; otherwise the current race or stage pod.
        const bool pointPod = world.stage.podInWorld(), racePod = world.race.podInWorld();
        if (!pointPod && !racePod) {
            blip(gHud.radarFix, world.rixDoorPos(), 0.005f, 44.0f, 424.0f);
            blip(gHud.radarRace, world.race.radarFlag(world.carsOwned), 0.005f, 44.0f, 424.0f);
            blip(gHud.radarStage, world.stage.flagPosition(), 0.005f, 44.0f, 424.0f);
        }
        if (pointPod) blip(gHud.radarPointPod, world.stage.podPosition(), 0.01f, 48.0f, 428.0f);
        if (racePod) blip(gHud.radarRacePod, world.race.podPosition(), 0.01f, 48.0f, 428.0f);
    }

    // InfoPanels.ls: panel overlay at (278,9), scale panelBlend / 70 from its
    // top-left registration point. Rising it is fully opaque; falling, the
    // blend follows panelBlend.
    if (h.panelBlend > 0) {
        const Texture *panel = hudPanel(h.messageID);
        if (panel && !panel->px.empty()) {
            const float scale = float(h.panelBlend) / 70.0f;
            const float alpha = h.panelUp ? 1.0f : float(h.panelBlend) / 100.0f;
            const StageScreen st(r);
            const float x = 278.0f * st.sx, y = 9.0f * st.sy;
            const float w = float(panel->w) / kPanelBakeScale * scale * st.sx;
            const float hgt = float(panel->h) / kPanelBakeScale * scale * st.sy;
            const float x0 = std::round(x), y0 = std::round(y);
            r.drawSprite(*panel, x0, y0, std::round(x + w) - x0, std::round(y + hgt) - y0, alpha);
        }
    }

    // FadeToBlack.ls: member 52 at (0,0), scale 100, blend fadeToBlackBlend.
    if (h.fadeBlend > 0) s.draw(r, gHud.fadeToBlack, 0.0f, 0.0f, 100.0f, float(h.fadeBlend) / 100.0f);
}

} // namespace rr4
