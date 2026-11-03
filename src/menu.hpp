#pragma once
// Main menu: a dark gradient panel on the left over the live cinematic city,
// the title, and DRIVE / RESUME, NEW GAME, SETTINGS and QUIT. NEW GAME asks
// for confirmation first. The settings screen edits a copy of Settings;
// main.cpp applies it. Title sprites and the font atlas come from
// tools/render_menu.py.

#include "renderer.hpp"
#include "settings.hpp"
#include "spritefont.hpp"

#include <string>
#include <vector>

namespace rr4 {

// Key presses this tick (edges, not held keys).
struct MenuInput {
    bool up = false, down = false, left = false, right = false;
    bool confirm = false, back = false;
};

class MainMenu {
public:
    enum class Action {
        None,
        Play,
        NewGame,
        Quit,
        SettingsChanged,   // a value changed; settings() holds it
        SettingsClosed,    // left the settings screen
    };

    bool load(const std::string &dir = "assets/hud/menu");
    // Show the menu; `sessionActive` turns DRIVE into RESUME.
    void open(bool sessionActive);
    // The settings shown on the settings screen and the resolutions to offer.
    void setSettings(const Settings &s, std::vector<Resolution> resolutions);
    const Settings &settings() const { return settings_; }
    Action update(const MenuInput &in);
    // `fade` (0..1) darkens the whole picture first (cinematic cuts).
    void draw(Renderer &r, float fade) const;

private:
    enum Item { kPlay, kNewGame, kSettings, kQuit, kItemCount };
    enum Row {
        kResolution, kDisplay, kVsync, kMsaa, kShadows, kDrawDistance, kTextures,
        kMusic, kSound, kBack, kRowCount
    };
    enum class Screen { Main, Confirm, Settings };

    Action updateSettings(const MenuInput &in);
    bool changeRow(int row, int dir);
    std::string rowLabel(int row) const;
    std::string rowValue(int row) const;
    void drawMain(Renderer &r, float ui) const;
    void drawSettings(Renderer &r, float ui) const;
    void drawConfirm(Renderer &r) const;

    Screen screen_ = Screen::Main;
    int selected_ = kPlay;
    int row_ = kResolution;
    bool session_ = false;
    int ticks_ = 0;            // since open, for the intro fade
    float highlightY_ = 0.0f;  // eased y of the selection bar

    Settings settings_;
    std::vector<Resolution> resolutions_;

    SpriteFont font_;
    Texture title_, subtitle_;
    Texture gradient_;   // left panel, built in load()
};

} // namespace rr4
