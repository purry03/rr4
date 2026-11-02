#pragma once
// Shared car-roster constants (MovieScript 1 - Initialization and the
// PlayerCar handlers that read g_CarsOwnedList / g_CarsColorIDList).

namespace rr4 {

constexpr int kCarCount = 6;    // cars 1..6
constexpr int kPaintJobs = 8;   // paint jobs 1..8 (Textures members 151..158)

// g_CarsOwnedList values.
enum Ownership : int {
    kNotOwned = 0,
    kOwnedParked = 1,    // waiting in a Ferris wheel cage
    kOwnedDriving = 2,   // the player's car
};

// Initialization defaults: g_CarsOwnedList = [2, 0, 0, 0, 0, 0],
// g_CarsColorIDList = [1, 2, 3, 4, 5, 1].
constexpr int kDefaultCarsOwned[kCarCount] = {kOwnedDriving, kNotOwned, kNotOwned,
                                              kNotOwned, kNotOwned, kNotOwned};
constexpr int kDefaultCarColors[kCarCount] = {1, 2, 3, 4, 5, 1};

} // namespace rr4
