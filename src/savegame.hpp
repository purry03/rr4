#pragma once
// Autosave: the original doSaveGameData / Initialization save list
// [g_CarsOwnedList, g_CarsColorIDList, g_CarsDamageList, g_CarsIgNitroList],
// kept in a plain "key = values" text file next to the executable.
//
//   # Redline Rumble 4 save
//   version = 1
//   cars_owned = 2 0 0 0 0 0
//   ...
//
// Unknown keys are ignored and missing keys keep their defaults, so new
// fields can be added without breaking older files.

#include "garage.hpp"

#include <filesystem>

namespace rr4 {

struct SaveGame {
    static constexpr int kVersion = 1;
    // Damage and igNitro are fractions: 0 = none / empty, 1 = wrecked / full.
    static constexpr float kEmpty = 0.0f, kFull = 1.0f;

    int carsOwned[kCarCount];    // g_CarsOwnedList (Ownership)
    int carColors[kCarCount];    // g_CarsColorIDList, paint job 1..kPaintJobs
    float carDamage[kCarCount];  // g_CarsDamageList
    float carIgNitro[kCarCount]; // g_CarsIgNitroList

    SaveGame();

    // The car being driven (kOwnedDriving), 1..kCarCount.
    int playerCar() const;

    // False when the file is missing or unreadable (defaults are kept).
    bool load(const std::filesystem::path &path);
    // Writes a temporary file and renames it over `path`, so a crash never
    // leaves a half-written save.
    bool save(const std::filesystem::path &path) const;
};

// kSaveFileName in the directory of the running executable.
constexpr const char *kSaveFileName = "rr4save.txt";
std::filesystem::path defaultSavePath();

} // namespace rr4
