#pragma once
// Player settings, kept in rr4settings.txt in userDataDir() in the
// same "key = value" format as the save (src/keyvalue.hpp).

#include <filesystem>

namespace rr4 {

// A screen size offered in the settings menu.
struct Resolution { int w, h; };

struct Settings {
    static constexpr int kVersion = 1;
    static constexpr int kMinWidth = 800, kMinHeight = 480;    // the original stage
    static constexpr int kMaxVolume = 100;
    static constexpr int kMsaaLevels[] = {0, 2, 4, 8};
    static constexpr int kVitaWidth = 960, kVitaHeight = 544, kVitaMaxMsaa = 4;

    int width = 1280, height = 800;
    bool fullscreen = false;
    bool vsync = true;
    int msaa = 4;                 // samples, one of kMsaaLevels
    bool shadows = true;
    int drawDistance = 3;         // tiles around the camera (World::k*DrawDistance)
    bool smoothTextures = true;   // bilinear + mipmaps, or nearest
    int musicVolume = kMaxVolume, sfxVolume = kMaxVolume;

    // False when the file is missing (defaults are kept).
    bool load(const std::filesystem::path &path);
    bool save(const std::filesystem::path &path) const;
    // Values outside their ranges are pulled back in.
    void clamp();
};

constexpr const char *kSettingsFileName = "rr4settings.txt";
std::filesystem::path defaultSettingsPath();

} // namespace rr4
