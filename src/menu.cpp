#include "menu.hpp"
#include "assets.hpp"
#include "hud.hpp"
#include "world.hpp"

#include <algorithm>
#include <iterator>

namespace rr4 {

namespace {

// Layout in 800 x 480 stage pixels. Title sprites are rendered at 2x.
constexpr float kSpriteScale = 0.5f;
constexpr float kStageW = 800.0f, kStageH = 480.0f;
constexpr float kPanelW = 400.0f;          // gradient panel width
constexpr float kSettingsPanelW = 560.0f;  // wider behind the setting values
constexpr int kGradientTexels = 256;
constexpr float kPanelAlpha = 0.8f;        // darkest (left edge)
constexpr float kMarginX = 40.0f;
constexpr float kTitleY = 60.0f;
constexpr float kItemsY = 205.0f, kItemStep = 46.0f, kItemSize = 40.0f;
constexpr float kRowsY = 128.0f, kRowStep = 28.0f, kRowSize = 25.0f;
constexpr float kValueX = 372.0f;          // right edge of setting values
constexpr float kHeadingY = 70.0f, kHeadingSize = 44.0f;
constexpr float kItemSlide = 12.0f;        // selected item moves right
constexpr float kAccentW = 4.0f;
constexpr float kDimAlpha = 0.5f;          // unselected items
constexpr float kHintY = 444.0f, kHintSize = 15.0f;
constexpr float kIntroTicks = 30.0f;       // UI fade in
constexpr float kHighlightEase = 0.25f;
constexpr float kDialogW = 460.0f, kDialogH = 160.0f;
constexpr int kVolumeStep = 10;
const uint32_t kBlack = packRGBA(0, 0, 0);
const uint32_t kWhite = packRGBA(255, 255, 255);
const uint32_t kGrey = packRGBA(190, 198, 210);
const uint32_t kPanel = packRGBA(10, 12, 18);
const uint32_t kAccent = packRGBA(255, 122, 26);

const char *kItemNames[] = {"DRIVE", "NEW GAME", "SETTINGS", "QUIT"};
const char *kDrawDistanceNames[] = {"ORIGINAL", "FAR", "FURTHER", "MAXIMUM"};

int wrap(int v, int n) { return ((v % n) + n) % n; }

} // namespace

bool MainMenu::load(const std::string &dir) {
    bool ok = font_.load(dir);
    for (auto [name, tex] : {std::pair{"Title", &title_}, std::pair{"Subtitle", &subtitle_}}) {
        ok &= loadTexture(dir + "/" + name + ".png", *tex);
        tex->buildMips();
    }
    // Panel gradient: kPanel, opaque at the left edge fading out to the right.
    gradient_.w = kGradientTexels;
    gradient_.h = 1;
    gradient_.px.resize(kGradientTexels);
    for (int i = 0; i < kGradientTexels; ++i) {
        const float t = float(i) / float(kGradientTexels - 1);
        gradient_.px[size_t(i)] = (kPanel & 0x00ffffffu) |
                                  (uint32_t(kPanelAlpha * (1.0f - t * t) * 255.0f) << 24);
    }
    return ok;
}

void MainMenu::open(bool sessionActive) {
    session_ = sessionActive;
    selected_ = kPlay;
    screen_ = Screen::Main;
    ticks_ = 0;
    highlightY_ = kItemsY;
}

void MainMenu::setSettings(const Settings &s, std::vector<Resolution> resolutions) {
    settings_ = s;
    resolutions_ = std::move(resolutions);
}

MainMenu::Action MainMenu::update(const MenuInput &in) {
    ++ticks_;
    switch (screen_) {
    case Screen::Confirm:
        if (in.confirm) {
            screen_ = Screen::Main;
            return Action::NewGame;
        }
        if (in.back) screen_ = Screen::Main;
        return Action::None;
    case Screen::Settings:
        highlightY_ += (kRowsY + kRowStep * float(row_) - highlightY_) * kHighlightEase;
        return updateSettings(in);
    case Screen::Main:
        break;
    }
    highlightY_ += (kItemsY + kItemStep * float(selected_) - highlightY_) * kHighlightEase;
    if (in.up) selected_ = wrap(selected_ - 1, kItemCount);
    if (in.down) selected_ = wrap(selected_ + 1, kItemCount);
    if (in.back && session_) return Action::Play;   // Esc goes back to the game
    if (!in.confirm) return Action::None;
    switch (selected_) {
    case kPlay: return Action::Play;
    case kNewGame: screen_ = Screen::Confirm; return Action::None;
    case kSettings:
        screen_ = Screen::Settings;
        row_ = kResolution;
        highlightY_ = kRowsY;
        return Action::None;
    default: return Action::Quit;
    }
}

MainMenu::Action MainMenu::updateSettings(const MenuInput &in) {
    if (in.back || (in.confirm && row_ == kBack)) {
        screen_ = Screen::Main;
        highlightY_ = kItemsY + kItemStep * float(selected_);
        return Action::SettingsClosed;
    }
    if (in.up) row_ = wrap(row_ - 1, kRowCount);
    if (in.down) row_ = wrap(row_ + 1, kRowCount);
    const int dir = in.right || in.confirm ? 1 : in.left ? -1 : 0;
    if (dir != 0 && changeRow(row_, dir)) return Action::SettingsChanged;
    return Action::None;
}

bool MainMenu::changeRow(int row, int dir) {
    Settings &s = settings_;
    switch (row) {
    case kResolution: {
        if (resolutions_.empty()) return false;
        int cur = 0;
        for (size_t i = 0; i < resolutions_.size(); ++i)
            if (resolutions_[i].w == s.width && resolutions_[i].h == s.height) cur = int(i);
        const Resolution &r = resolutions_[size_t(wrap(cur + dir, int(resolutions_.size())))];
        s.width = r.w;
        s.height = r.h;
        return true;
    }
    case kDisplay: s.fullscreen = !s.fullscreen; return true;
    case kVsync: s.vsync = !s.vsync; return true;
    case kMsaa: {
        const int n = int(std::size(Settings::kMsaaLevels));
        int cur = int(std::find(std::begin(Settings::kMsaaLevels), std::end(Settings::kMsaaLevels),
                                s.msaa) - std::begin(Settings::kMsaaLevels));
        s.msaa = Settings::kMsaaLevels[wrap(cur + dir, n)];
        return true;
    }
    case kShadows: s.shadows = !s.shadows; return true;
    case kDrawDistance: {
        const int n = World::kMaxDrawDistance - World::kMinDrawDistance + 1;
        s.drawDistance = World::kMinDrawDistance + wrap(s.drawDistance - World::kMinDrawDistance + dir, n);
        return true;
    }
    case kTextures: s.smoothTextures = !s.smoothTextures; return true;
    case kMusic:
        s.musicVolume = std::clamp(s.musicVolume + dir * kVolumeStep, 0, Settings::kMaxVolume);
        return true;
    case kSound:
        s.sfxVolume = std::clamp(s.sfxVolume + dir * kVolumeStep, 0, Settings::kMaxVolume);
        return true;
    default: return false;
    }
}

std::string MainMenu::rowLabel(int row) const {
    static const char *names[kRowCount] = {"RESOLUTION", "DISPLAY", "VSYNC", "ANTI-ALIASING",
                                           "SHADOWS", "DRAW DISTANCE", "TEXTURES", "MUSIC",
                                           "SOUND", "BACK"};
    return names[row];
}

std::string MainMenu::rowValue(int row) const {
    const Settings &s = settings_;
    auto onOff = [](bool b) { return std::string(b ? "ON" : "OFF"); };
    switch (row) {
    case kResolution: return std::to_string(s.width) + " X " + std::to_string(s.height);
    case kDisplay: return s.fullscreen ? "FULLSCREEN" : "WINDOWED";
    case kVsync: return onOff(s.vsync);
    case kMsaa: return s.msaa ? std::to_string(s.msaa) + "X" : "OFF";
    case kShadows: return onOff(s.shadows);
    case kDrawDistance: return kDrawDistanceNames[s.drawDistance - World::kMinDrawDistance];
    case kTextures: return s.smoothTextures ? "SMOOTH" : "PIXELATED";
    case kMusic: return std::to_string(s.musicVolume) + "%";
    case kSound: return std::to_string(s.sfxVolume) + "%";
    default: return "";
    }
}

void MainMenu::draw(Renderer &r, float fade) const {
    if (fade > 0.0f) drawStageRect(r, 0, 0, kStageW, kStageH, kBlack, fade);
    const float ui = std::min(1.0f, float(ticks_) / kIntroTicks);
    drawStageSpriteRect(r, gradient_, 0, 0, screen_ == Screen::Settings ? kSettingsPanelW : kPanelW,
                        kStageH, ui);
    if (screen_ == Screen::Settings) drawSettings(r, ui);
    else drawMain(r, ui);
    if (screen_ == Screen::Confirm) drawConfirm(r);
}

void MainMenu::drawMain(Renderer &r, float ui) const {
    drawStageSprite(r, title_, kMarginX - 6.0f, kTitleY, kSpriteScale, ui);
    drawStageSprite(r, subtitle_, kMarginX - 4.0f, kTitleY + float(title_.h) * kSpriteScale - 6.0f,
                    kSpriteScale, ui);
    drawStageRect(r, kMarginX - 14.0f, highlightY_ + kItemSize * 0.22f, kAccentW,
                  kItemSize * 0.5f, kAccent, ui);
    for (int i = 0; i < kItemCount; ++i) {
        const bool sel = i == selected_ && screen_ == Screen::Main;
        const char *name = i == kPlay && session_ ? "RESUME" : kItemNames[i];
        font_.draw(r, name, kMarginX + (sel ? kItemSlide : 0.0f), kItemsY + kItemStep * float(i),
                   kItemSize, kWhite, ui * (sel ? 1.0f : kDimAlpha));
    }
    font_.draw(r, "UP / DOWN  SELECT     ENTER  CONFIRM", kMarginX, kHintY, kHintSize, kGrey,
               ui * kDimAlpha);
}

void MainMenu::drawSettings(Renderer &r, float ui) const {
    font_.draw(r, "SETTINGS", kMarginX, kHeadingY, kHeadingSize, kWhite, ui);
    drawStageRect(r, kMarginX - 14.0f, highlightY_ + kRowSize * 0.22f, kAccentW, kRowSize * 0.5f,
                  kAccent, ui);
    for (int i = 0; i < kRowCount; ++i) {
        const bool sel = i == row_;
        const float y = kRowsY + kRowStep * float(i);
        const float a = ui * (sel ? 1.0f : kDimAlpha);
        font_.draw(r, rowLabel(i), kMarginX + (sel ? kItemSlide * 0.5f : 0.0f), y, kRowSize,
                   kWhite, a);
        std::string value = rowValue(i);
        if (value.empty()) continue;
        if (sel) value = "< " + value + " >";
        font_.draw(r, value, kValueX, y, kRowSize, sel ? kAccent : kGrey, a, SpriteFont::Align::Right);
    }
    font_.draw(r, "LEFT / RIGHT  CHANGE     ESC  BACK", kMarginX, kHintY, kHintSize, kGrey,
               ui * kDimAlpha);
}

void MainMenu::drawConfirm(Renderer &r) const {
    drawStageRect(r, 0, 0, kStageW, kStageH, kBlack, 0.55f);
    const float dx = (kStageW - kDialogW) * 0.5f, dy = (kStageH - kDialogH) * 0.5f;
    const float cx = kStageW * 0.5f;
    drawStageRect(r, dx, dy, kDialogW, kDialogH, kPanel, 0.92f);
    drawStageRect(r, dx, dy, kDialogW, 3.0f, kAccent, 1.0f);
    font_.draw(r, "START A NEW GAME?", cx, dy + 22.0f, 36.0f, kWhite, 1.0f, SpriteFont::Align::Centre);
    font_.draw(r, "YOUR CARS, PAINT AND PROGRESS WILL BE RESET.", cx, dy + 72.0f, 17.0f, kGrey, 1.0f,
               SpriteFont::Align::Centre);
    font_.draw(r, "ENTER  YES      ESC  NO", cx, dy + 108.0f, 18.0f, kAccent, 1.0f,
               SpriteFont::Align::Centre);
}

} // namespace rr4
