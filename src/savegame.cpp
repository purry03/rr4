#include "savegame.hpp"
#include "keyvalue.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace rr4 {

namespace {

// set the floatPrecision to 2 (doSaveGameData).
constexpr int kSavedDecimals = 2;

// Reads up to kCarCount numbers; values outside [lo, hi] leave the default.
template <class T>
void readList(const std::string &text, T (&out)[kCarCount], T lo, T hi) {
    std::istringstream in(text);
    for (T &v : out) {
        T x;
        if (!(in >> x)) break;
        if (x >= lo && x <= hi) v = x;
    }
}

template <class T>
void writeList(std::ostream &out, const char *key, const T (&v)[kCarCount]) {
    out << key << " =";
    for (T x : v) out << ' ' << x;
    out << '\n';
}

} // namespace

SaveGame::SaveGame() {
    for (int i = 0; i < kCarCount; ++i) {
        carsOwned[i] = kDefaultCarsOwned[i];
        carColors[i] = kDefaultCarColors[i];
        carDamage[i] = kEmpty;
        carIgNitro[i] = kFull;
    }
}

int SaveGame::playerCar() const {
    for (int i = 0; i < kCarCount; ++i)
        if (carsOwned[i] == kOwnedDriving) return i + 1;
    return 1;
}

bool SaveGame::load(const std::filesystem::path &path) {
    KeyValues kv;
    if (!readKeyValues(path, kv)) return false;
    auto list = [&](const char *key, auto &out, auto lo, auto hi) {
        if (auto it = kv.find(key); it != kv.end()) readList(it->second, out, lo, hi);
    };
    list("cars_owned", carsOwned, int(kNotOwned), int(kOwnedDriving));
    list("car_colors", carColors, 1, kPaintJobs);
    list("car_damage", carDamage, kEmpty, kFull);
    list("car_ignitro", carIgNitro, kEmpty, kFull);
    // Exactly one car is driven; fall back to car 1 if the file disagrees.
    if (std::count(carsOwned, carsOwned + kCarCount, int(kOwnedDriving)) != 1) {
        for (int &o : carsOwned) if (o == kOwnedDriving) o = kOwnedParked;
        carsOwned[0] = kOwnedDriving;
    }
    return true;
}

bool SaveGame::save(const std::filesystem::path &path) const {
    std::ostringstream out;
    out << "# Redline Rumble 4 save\n";
    out << "# cars 1..6: MeanieCropper Bitsasushi Amazed HighWire Posher Furore\n";
    out << "# cars_owned: 2 = driving, 1 = parked at Ferris' Wheels, 0 = not owned\n";
    out << "version = " << kVersion << '\n';
    writeList(out, "cars_owned", carsOwned);
    writeList(out, "car_colors", carColors);
    out.setf(std::ios::fixed);
    out.precision(kSavedDecimals);
    writeList(out, "car_damage", carDamage);
    writeList(out, "car_ignitro", carIgNitro);
    return writeFileAtomic(path, out.str());
}

std::filesystem::path defaultSavePath() { return executableDir() / kSaveFileName; }
} // namespace rr4
