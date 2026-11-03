#include "settings.hpp"
#include "keyvalue.hpp"
#include "world.hpp"

#include <algorithm>
#include <sstream>
#include <string>

namespace rr4 {

namespace {
bool parseBool(const std::string &v) { return v == "1" || v == "true" || v == "on" || v == "yes"; }
} // namespace

bool Settings::load(const std::filesystem::path &path) {
    KeyValues kv;
    if (!readKeyValues(path, kv)) return false;
    auto num = [&](const char *key, int &out) {
        auto it = kv.find(key);
        if (it == kv.end()) return;
        try { out = std::stoi(it->second); } catch (...) {}
    };
    auto flag = [&](const char *key, bool &out) {
        if (auto it = kv.find(key); it != kv.end()) out = parseBool(it->second);
    };
    if (auto it = kv.find("resolution"); it != kv.end()) {
        int w = 0, h = 0;
        char x = 0;
        std::istringstream in(it->second);
        if (in >> w >> x >> h && (x == 'x' || x == 'X')) { width = w; height = h; }
    }
    flag("fullscreen", fullscreen);
    flag("vsync", vsync);
    num("msaa", msaa);
    flag("shadows", shadows);
    num("draw_distance", drawDistance);
    flag("smooth_textures", smoothTextures);
    num("music_volume", musicVolume);
    num("sfx_volume", sfxVolume);
    clamp();
    return true;
}

void Settings::clamp() {
#ifdef __vita__
    // One fixed screen; vitaGL multisamples up to 4x.
    width = kVitaWidth;
    height = kVitaHeight;
    fullscreen = true;
    msaa = std::min(msaa, kVitaMaxMsaa);
#endif
    width = std::max(width, kMinWidth);
    height = std::max(height, kMinHeight);
    if (std::find(std::begin(kMsaaLevels), std::end(kMsaaLevels), msaa) == std::end(kMsaaLevels))
        msaa = 0;
    drawDistance = std::clamp(drawDistance, World::kMinDrawDistance, World::kMaxDrawDistance);
    musicVolume = std::clamp(musicVolume, 0, kMaxVolume);
    sfxVolume = std::clamp(sfxVolume, 0, kMaxVolume);
}

bool Settings::save(const std::filesystem::path &path) const {
    auto onOff = [](bool b) { return b ? "on" : "off"; };
    std::ostringstream out;
    out << "# Redline Rumble 4 settings\n";
    out << "version = " << kVersion << '\n';
    out << "resolution = " << width << 'x' << height << '\n';
    out << "fullscreen = " << onOff(fullscreen) << '\n';
    out << "vsync = " << onOff(vsync) << '\n';
    out << "msaa = " << msaa << '\n';
    out << "shadows = " << onOff(shadows) << '\n';
    out << "# tiles drawn around the camera, " << World::kMinDrawDistance << ".."
        << World::kMaxDrawDistance << " (the original draws 2)\n";
    out << "draw_distance = " << drawDistance << '\n';
    out << "smooth_textures = " << onOff(smoothTextures) << '\n';
    out << "music_volume = " << musicVolume << '\n';
    out << "sfx_volume = " << sfxVolume << '\n';
    return writeFileAtomic(path, out.str());
}

std::filesystem::path defaultSettingsPath() { return userDataDir() / kSettingsFileName; }

} // namespace rr4
