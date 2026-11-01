#include "hud.hpp"
#include "car.hpp"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <vector>

namespace rr4 {

HudAssets gHud;

namespace {

// Original movie stage used by every camera overlay coordinate.
constexpr float kStageW = 800.0f;
constexpr float kStageH = 480.0f;

bool loadTexture(const std::string &path, Texture &out) {
    sf::Image img;
    if (!img.loadFromFile(path)) {
        std::fprintf(stderr, "hud: missing %s\n", path.c_str());
        return false;
    }
    auto size = img.getSize();
    out.w = int(size.x);
    out.h = int(size.y);
    out.px.resize(size_t(out.w) * out.h);
    const uint8_t *src = img.getPixelsPtr();
    for (size_t i = 0; i < out.px.size(); ++i)
        out.px[i] = packRGBA(src[i * 4], src[i * 4 + 1], src[i * 4 + 2], src[i * 4 + 3]);
    return true;
}

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

void drawHud(Renderer &r, const Car &car, bool muted) {
    if (!gHud.loaded && !loadHudAssets()) return;
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
    // tenths/hundredths at 756/772 with scale 0.5 (g_TimerState = 0 in free
    // drive, so the display stays at 00:00.00).
    int timer[5] = {0, 0, 0, 0, 0};
    s.draw(r, gHud.timer[timer[0]], 640.0f, 28.0f);
    s.draw(r, gHud.timerColon, 664.0f, 28.0f);
    s.draw(r, gHud.timer[timer[1]], 692.0f, 28.0f);
    s.draw(r, gHud.timer[timer[2]], 718.0f, 28.0f);
    s.draw(r, gHud.timer[timer[3]], 756.0f, 30.0f, 0.5f);
    s.draw(r, gHud.timer[timer[4]], 772.0f, 30.0f, 0.5f);

    // Overlays.ls: damage bar at (0,8), red fill at -254 + 254 * damage.
    // The original eases displayDamagePercent 20% per frame.
    static float displayDamage = 0.0f;
    displayDamage += (clampf(car.damage, 0.0f, 1.0f) - displayDamage) * 0.2f;
    float damage = displayDamage;
    s.draw(r, gHud.damageBar, 0.0f, 8.0f);
    s.draw(r, gHud.damageRed, -254.0f + damage * 254.0f, 8.0f);
    s.draw(r, gHud.skull, 214.0f, 0.0f);

    // boost fill anchor: 800 - 250 * g_BoostPercent.
    static float displayBoost = 1.0f;
    displayBoost += (clampf(car.boostMeter, 0.0f, 1.0f) - displayBoost) * 0.2f;
    float boost = displayBoost;
    s.draw(r, gHud.boostBar, 544.0f, 8.0f);
    s.draw(r, gHud.boostYellow, 800.0f - boost * 250.0f, 8.0f);
    s.draw(r, gHud.boostLogo, 505.0f, 2.0f);

    // MUTE+PAUSE_Button.ls: speaker icon at (776,460).
    s.draw(r, muted ? gHud.muteOn : gHud.muteOff, 776.0f, 460.0f);

    // doSetupRadar(): radar ring at (20,400). The pod icons only appear when
    // the race/stage systems place them; free drive leaves them hidden.
    s.draw(r, gHud.radarRing, 20.0f, 400.0f);
}

} // namespace rr4
