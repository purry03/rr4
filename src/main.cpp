#include "audio.hpp"
#include "car.hpp"
#include "font.hpp"
#include "hud.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "world.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace rr4;

namespace {

constexpr int kWidth = 640;
constexpr int kHeight = 400;
constexpr int kWindowWidth = 960;
constexpr int kWindowHeight = 600;

struct ChaseCam {
    Camera cam;
    Vec3 aim;
    float range = 200.0f;   // doSetupPlayerCar: camRange = 200
    float height = 50.0f;   // doSetupPlayerCar: camHeight = 50
    int state = 1;          // camState
    int lookingForwards = 0;
    bool init = false;

    // doCam() from BehaviorScript 4 - PlayerCar.ls.
    void update(const Car &car, int tileID, float factor) {
        if (!init) {
            // doSetupPlayerCar: camAim and cam.worldPosition around the car.
            aim = car.pos + Vec3(-80, 50, 200);
            cam.pos = car.pos + Vec3(-80, 50, -200);
            init = true;
            return;
        }

        if (car.throttle > 6.0f) lookingForwards = 1;
        if (car.throttle < -7.0f) lookingForwards = -1;
        if (car.throttle < 0.0f) lookingForwards = 0;

        Vec3 toCam = cam.pos - car.pos;
        toCam.y = 0;
        if (toCam.length() > 1e-4f) toCam = toCam.normalized();
        Vec3 side = -car.rightAxis();
        side.y = 0;
        if (side.length() > 1e-4f) side = side.normalized();
        Vec3 front = -car.forward();
        front.y = 0;
        if (front.length() > 1e-4f) front = front.normalized();
        float frontSign = front.dot(toCam);
        float tempRot = side.dot(toCam);
        if (lookingForwards > 0 && frontSign < 0)
            tempRot = tempRot > 0 ? 1.0f : -1.0f;
        if (lookingForwards > 0) tempRot = -tempRot;
        else tempRot = 0;

        Vec3 offset(cam.pos.x - car.pos.x, 0, cam.pos.z - car.pos.z);
        if (offset.length() > 1e-4f) {
            offset = offset.normalized();
            if (tempRot != 0.0f)
                offset = Mat4::rotationY(deg2rad(tempRot * factor)).transformVector(offset);
        }

        // camState: tile 7 and tile 20 pull the camera up and back.
        state = (tileID == 7 || tileID == 20) ? 2 : 1;
        float targetRange = state == 2 ? 480.0f : 320.0f;
        float targetHeight = state == 2 ? 200.0f : 120.0f;
        float rangeRate = 1.0f - std::pow(0.95f, factor);
        float aimRate = 1.0f - std::pow(0.90f, factor);

        range += (targetRange - range) * rangeRate;
        height += (targetHeight - height) * rangeRate;
        Vec3 target = car.pos + car.vel * factor + Vec3(0, 90, 0);
        aim += (target - aim) * aimRate;
        aim.x = car.pos.x;
        aim.z = car.pos.z;

        cam.pos.y += (car.pos.y + height - cam.pos.y) * aimRate;
        cam.pos.x = car.pos.x + offset.x * range;
        cam.pos.z = car.pos.z + offset.z * range;
        cam.target = aim;
        cam.fovDeg = 55.0f;
        cam.up = {0, 1, 0};
    }
};

// Optional development readout, off by default so the screen is exactly the
// original HUD (F1 toggles it).
void drawDebugHud(Renderer &r, const World &world, const Car &car, float fps) {
    blendRect(r, 8, 64, 300, 96, packRGBA(6, 10, 16), 0.62f);
    char buf[160];
    std::snprintf(buf, sizeof(buf), "SPEED %3.0f KM/H   FPS %3.0f", car.speedKmh, fps);
    drawText(r, 14, 70, buf, packRGBA(255, 255, 255), 1);
    std::snprintf(buf, sizeof(buf), "THROTTLE %+5.1f  STEER %+5.1f", car.throttle, car.steer);
    drawText(r, 14, 82, buf, packRGBA(180, 220, 190), 1);
    std::snprintf(buf, sizeof(buf), "POS X %.0f  Z %.0f", car.pos.x, car.pos.z);
    drawText(r, 14, 94, buf, packRGBA(180, 200, 214), 1);
    int id = world.tileIDAt(car.pos.x, car.pos.z);
    std::snprintf(buf, sizeof(buf), "TILE %02d %s", id, tileName(id));
    drawText(r, 14, 106, buf, packRGBA(200, 214, 226), 1);
    std::snprintf(buf, sizeof(buf), "TILT %+5.1f / %+5.1f  BOOST %.0f%%",
                  car.surfacePitchDeg, car.surfaceRollDeg, car.boostMeter * 100.0f);
    drawText(r, 14, 118, buf, packRGBA(200, 214, 226), 1);
    drawText(r, 14, 134, "ARROWS DRIVE  SHIFT BOOST  M MUTE  F1 DEBUG",
             packRGBA(150, 170, 190), 1);
}

void renderFrame(Renderer &r, World &world, Car &car, ChaseCam &chase, float fps,
                 bool muted = false) {
    r.clearSky(chase.cam.pos);
    world.draw(r, chase.cam);
    car.draw(r, chase.cam);
    drawHud(r, car, muted);
}

bool saveFrame(Renderer &r, const fs::path &path) {
    fs::create_directories(path.parent_path());
    sf::Image img(sf::Vector2u(unsigned(r.width), unsigned(r.height)),
                  reinterpret_cast<const std::uint8_t *>(r.color.data()));
    return img.saveToFile(path);
}

// Waypoint autopilot used only for headless verification: it follows the
// original road loop so screenshots prove the car can drive the whole map.
struct Waypoint { float x, z; };

const Waypoint kWaypoints[] = {
    {52000, 31000}, {55000, 30000}, {55000, 12000}, {51000, 10000},
    {24000, 12000}, {16000, 10000}, {15000, 14000}, {15000, 44000},
    {30000, 45000}, {31000, 42000}, {32000, 32000},
};

float wrapAngle(float a) {
    while (a > PI) a -= 2 * PI;
    while (a < -PI) a += 2 * PI;
    return a;
}

CarInput autoInput(const Car &car, int &wpIndex, int frame, int &stuckFrames) {
    CarInput in;
    const int count = int(sizeof(kWaypoints) / sizeof(kWaypoints[0]));
    Waypoint wp = kWaypoints[wpIndex];
    float dx = wp.x - car.pos.x;
    float dz = wp.z - car.pos.z;
    float dist = std::sqrt(dx * dx + dz * dz);
    if (dist < 1500.0f) {
        wpIndex = (wpIndex + 1) % count;
        wp = kWaypoints[wpIndex];
        dx = wp.x - car.pos.x;
        dz = wp.z - car.pos.z;
    }

    float desired = std::atan2(dx, dz);
    float err = wrapAngle(desired - car.yaw);
    if (err > 0.05f) in.left = true;
    if (err < -0.05f) in.right = true;

    float fwdSpeed = car.forwardSpeed();
    if (frame > 30 && std::fabs(fwdSpeed) < 2.0f) {
        ++stuckFrames;
    } else {
        stuckFrames = 0;
    }
    if (stuckFrames > 60 && stuckFrames < 140) {
        // Back out of whatever we nudged, steering the other way.
        in.up = false;
        in.down = true;
        std::swap(in.left, in.right);
        return in;
    }
    if (std::fabs(err) > 0.85f && fwdSpeed > 14.0f) {
        in.down = true; // brake into tight corners
    } else if (fwdSpeed < 34.0f) {
        in.up = true;
    }
    // use the nitro on long straights
    in.boost = std::fabs(err) < 0.04f && fwdSpeed > 22.0f && (frame % 420) < 180;
    return in;
}

int runAuto(const std::string &shotDir, int totalFrames, int carId, int shotEvery,
            bool place, float px, float pz, float pyaw) {
    Renderer r(kWidth, kHeight);
    World world;
    world.build();
    buildCarAssets();

    Car car;
    car.setCar(carId);
    if (place) {
        car.pos = {px, 0.0f, pz};
        car.yaw = deg2rad(pyaw);
        world.resolveCarCollision(car);
    }
    ChaseCam chase;
    chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);

    fs::create_directories(shotDir);
    int saved = 0;
    int wpIndex = 0;
    int stuckFrames = 0;

    std::printf("auto-drive: %d frames at 60 Hz\n", totalFrames);
    std::printf("start pos=(%.2f, %.2f, %.2f) yaw=%.1f deg tile=%d (%s)\n",
                car.pos.x, car.pos.y, car.pos.z, car.yaw * 180.0f / PI,
                world.tileIDAt(car.pos.x, car.pos.z),
                tileName(world.tileIDAt(car.pos.x, car.pos.z)));

    float maxSpeed = 0.0f;
    float minX = 1e9f, maxX = -1e9f, minZ = 1e9f, maxZ = -1e9f;

    for (int frame = 0; frame <= totalFrames; ++frame) {
        CarInput in = autoInput(car, wpIndex, frame, stuckFrames);
        car.update(in, 1.0f);
        world.resolveCarCollision(car);
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);

        if (frame % 60 == 0) {
            std::printf("frame %4d  pos=(%8.1f,%8.1f) yaw=%6.1f  speed=%5.1f km/h  "
                        "tilt=%+5.1f/%+5.1f  tile=%02d %s\n",
                        frame, car.pos.x, car.pos.z, car.yaw * 180.0f / PI,
                        car.speedKmh, car.surfacePitchDeg, car.surfaceRollDeg,
                        world.tileIDAt(car.pos.x, car.pos.z),
                        tileName(world.tileIDAt(car.pos.x, car.pos.z)));
            std::fflush(stdout);
        }

        maxSpeed = std::max(maxSpeed, car.speedKmh);
        minX = std::min(minX, car.pos.x); maxX = std::max(maxX, car.pos.x);
        minZ = std::min(minZ, car.pos.z); maxZ = std::max(maxZ, car.pos.z);

        bool shoot = (shotEvery > 0 && frame % shotEvery == 0) || frame == totalFrames;
        if (shoot) {
            renderFrame(r, world, car, chase, 60.0f);
            char name[64];
            std::snprintf(name, sizeof(name), "frame_%04d.png", frame);
            fs::path p = fs::path(shotDir) / name;
            if (saveFrame(r, p)) {
                std::printf("saved %s\n", p.string().c_str());
                ++saved;
            } else {
                std::printf("FAILED to save %s\n", p.string().c_str());
            }
        }
    }
    std::printf("route: x %.0f..%.0f  z %.0f..%.0f  max speed %.1f km/h\n",
                minX, maxX, minZ, maxZ, maxSpeed);
    std::printf("shots saved: %d\n", saved);
    return saved > 0 ? 0 : 1;
}

CarInput traceInput(int frame) {
    CarInput in;
    in.up = true;
    if (frame >= 120 && frame < 180) in.left = true;
    if (frame >= 240 && frame < 300) in.right = true;
    return in;
}

int runTrace(int frames, int carId) {
    Car car;
    car.setCar(carId);
    std::printf("frame,throttle,steer,yawDeg,yawVel,velX,velY,velZ,posX,posY,posZ,rotX,rotZ\n");
    for (int f = 0; f <= frames; ++f) {
        CarInput in = traceInput(f);
        car.update(in, 1.0f);
        std::printf("%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"
                    "%.6f,%.6f\n",
                    f, car.throttle, car.steer, car.yaw * 180.0f / PI, car.yawVelDeg,
                    car.vel.x, car.vel.y, car.vel.z, car.pos.x, car.pos.y, car.pos.z,
                    car.rotXDeg, car.rotZDeg);
    }
    return 0;
}

int runCamTrace(int frames, int carId) {
    World world; // tile lookup only, no packed assets needed
    Car car;
    car.setCar(carId);
    ChaseCam chase;
    chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
    int wpIndex = 0, stuckFrames = 0;

    std::printf("frame,tile,throttle,yawDeg,posX,posY,posZ,velX,velY,velZ,"
                "range,height,aimX,aimY,aimZ,camX,camY,camZ\n");
    for (int f = 0; f <= frames; ++f) {
        CarInput in = autoInput(car, wpIndex, f, stuckFrames);
        car.update(in, 1.0f);
        world.resolveCarCollision(car);
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
        std::printf("%d,%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"
                    "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                    f, world.tileIDAt(car.pos.x, car.pos.z), car.throttle,
                    car.yaw * 180.0f / PI, car.pos.x, car.pos.y, car.pos.z,
                    car.vel.x, car.vel.y, car.vel.z, chase.range, chase.height,
                    chase.aim.x, chase.aim.y, chase.aim.z, chase.cam.pos.x,
                    chase.cam.pos.y, chase.cam.pos.z);
    }
    return 0;
}

int runBench(int frames, int carId) {
    Renderer r(kWidth, kHeight);
    World world;
    world.build();
    buildCarAssets();

    Car car;
    car.setCar(carId);
    ChaseCam chase;
    chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
    int wpIndex = 0, stuckFrames = 0;

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < frames; ++i) {
        CarInput in = autoInput(car, wpIndex, i, stuckFrames);
        car.update(in, 1.0f);
        world.resolveCarCollision(car);
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
        renderFrame(r, world, car, chase, 60.0f);
    }
    auto t1 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count() / frames;
    std::printf("bench: %d frames, %.2f ms/frame, %.1f fps (software rasterizer, %dx%d)\n",
                frames, ms, 1000.0 / ms, kWidth, kHeight);
    saveFrame(r, "work/bench_last.png");
    return 0;
}

int runAudioTest(int carId) {
    Audio audio;
    if (!audio.load()) return 1;
    audio.setCar(carId);
    audio.startMusic();
    Car car;
    car.setCar(carId);
    CarInput in;
    // Simulate the doEngineSounds state machine, then a brake and a boost.
    for (int i = 0; i < 90; ++i) {
        in.up = i < 40;
        audio.tickEngine(in.up);
        car.update(in, 1.0f);
        audio.tickCar(car, in);
        sf::sleep(sf::milliseconds(10));
    }
    in = {};
    in.down = true;
    audio.tickEngine(false);
    car.update(in, 1.0f);
    audio.tickCar(car, in);
    sf::sleep(sf::milliseconds(150));
    in = {};
    in.boost = true;
    audio.tickEngine(false);
    car.update(in, 1.0f);
    audio.tickCar(car, in);
    sf::sleep(sf::milliseconds(150));
    audio.toggleMute();
    audio.toggleMute();
    std::printf("audio-test: engine/music/brake/nitro/mute path complete\n");
    return 0;
}

// Development aid: report the settled ground height and surface tilt at one
// spot, so pitch/roll signs can be checked against a known bank.
int runProbe(float x, float z, float yawDeg, int carId) {
    World world;
    world.build();
    Car car;
    car.setCar(carId);
    car.pos = {x, 0.0f, z};
    car.yaw = deg2rad(yawDeg);
    for (int i = 0; i < 60; ++i) world.resolveCarCollision(car);
    int id = world.tileIDAt(car.pos.x, car.pos.z);
    std::printf("probe x=%.1f z=%.1f yaw=%.1f car=%d tile=%02d %s y=%.2f pitch=%+.2f roll=%+.2f\n",
                car.pos.x, car.pos.z, yawDeg, carId, id, tileName(id),
                car.pos.y, car.surfacePitchDeg, car.surfaceRollDeg);
    const CarConfig &cfg = carConfig(carId);
    Vec3 fwd = car.forward(), axisX = car.rightAxis();
    float front = cfg.tirePivot.z;
    float rear = cfg.tirePivot.z + cfg.rearAxleOffset.z;
    float halfTrack = cfg.tirePivot.x + 7.5f;
    auto sample = [&](float lat, float lon) {
        Vec3 p = car.pos + fwd * lon + axisX * lat;
        return world.groundHeightAt(p.x, p.z, car.pos.y - 50.0f);
    };
    std::printf("  wheels (local +x = left, +z = nose):\n"
                "    front-left  %8.2f  front-right %8.2f\n"
                "    rear-left   %8.2f  rear-right  %8.2f\n",
                sample(halfTrack, front), sample(-halfTrack, front),
                sample(halfTrack, rear), sample(-halfTrack, rear));
    return 0;
}

// Development aid: find the steepest original road surfaces on the 12 x 12
// map (banked NurdBurger corners, bridge ramps, canal slopes).
int runSlopeScan() {
    World world;
    world.build();
    Car car;
    car.setCar(1);
    struct Hit { float x, z, y, pitch, roll; int id; };
    std::vector<Hit> hits;
    for (int z = 2500; z <= 60000; z += 250) {
        for (int x = 0; x <= 60000; x += 250) {
            int id = world.tileIDAt(float(x), float(z));
            if (!isRoadTile(id)) continue;
            car.pos = {float(x), 0.0f, float(z)};
            car.yaw = 0.0f;
            car.surfacePitchDeg = car.surfaceRollDeg = 0.0f;
            world.resolveCarCollision(car);
            float score = std::max(std::fabs(car.surfacePitchDeg),
                                   std::fabs(car.surfaceRollDeg));
            if (score > 1.0f && score < 4.0f) hits.push_back({float(x), float(z), car.pos.y,
                                              car.surfacePitchDeg, car.surfaceRollDeg, id});
        }
    }
    std::sort(hits.begin(), hits.end(), [](const Hit &a, const Hit &b) {
        return std::max(std::fabs(a.pitch), std::fabs(a.roll)) >
               std::max(std::fabs(b.pitch), std::fabs(b.roll));
    });
    std::printf("slope-scan: %zu steep spots\n", hits.size());
    for (size_t i = 0; i < hits.size() && i < 25; ++i) {
        const Hit &h = hits[i];
        std::printf("  x=%6.0f z=%6.0f y=%8.2f pitch=%+6.2f roll=%+6.2f tile=%02d %s\n",
                    h.x, h.z, h.y, h.pitch, h.roll, h.id, tileName(h.id));
    }
    return 0;
}

} // namespace

int main(int argc, char **argv) {
    gSmoothTextures = std::getenv("RR4_NEAREST") == nullptr;
    int autoFrames = 0;
    int benchFrames = 0;
    int traceFrames = -1;
    int camTraceFrames = -1;
    std::string shotDir = "work/shots";
    bool autoMode = false;
    bool audioTest = false;
    bool slopeScan = false;
    bool probe = false;
    bool place = false;
    float probeX = 0.0f, probeZ = 0.0f, probeYaw = 0.0f;
    float placeX = 0.0f, placeZ = 0.0f, placeYaw = 0.0f;
    int shotEvery = 300;
    int carId = 1; // original default save owns MeanieCropper (car #1)

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--auto") {
            autoMode = true;
            if (i + 1 < argc) autoFrames = std::atoi(argv[++i]);
            if (autoFrames <= 0) autoFrames = 900;
        } else if (a == "--shots" && i + 1 < argc) {
            shotDir = argv[++i];
        } else if (a == "--trace") {
            traceFrames = 300;
            if (i + 1 < argc && argv[i + 1][0] != '-') traceFrames = std::atoi(argv[++i]);
        } else if (a == "--cam-trace") {
            camTraceFrames = 600;
            if (i + 1 < argc && argv[i + 1][0] != '-') camTraceFrames = std::atoi(argv[++i]);
        } else if (a == "--bench") {
            benchFrames = 120;
            if (i + 1 < argc && argv[i + 1][0] != '-') benchFrames = std::atoi(argv[++i]);
        } else if (a == "--audio-test") {
            audioTest = true;
        } else if (a == "--car" && i + 1 < argc) {
            carId = std::atoi(argv[++i]);
        } else if (a == "--shot-every" && i + 1 < argc) {
            shotEvery = std::atoi(argv[++i]);
        } else if (a == "--slope-scan") {
            slopeScan = true;
        } else if (a == "--place" && i + 3 < argc) {
            place = true;
            placeX = float(std::atof(argv[++i]));
            placeZ = float(std::atof(argv[++i]));
            placeYaw = float(std::atof(argv[++i]));
        } else if (a == "--probe" && i + 3 < argc) {
            probe = true;
            probeX = float(std::atof(argv[++i]));
            probeZ = float(std::atof(argv[++i]));
            probeYaw = float(std::atof(argv[++i]));
        }
    }

    if (slopeScan) return runSlopeScan();
    if (probe) return runProbe(probeX, probeZ, probeYaw, carId);
    if (audioTest) return runAudioTest(carId);
    if (camTraceFrames >= 0) return runCamTrace(camTraceFrames, carId);
    if (traceFrames >= 0) return runTrace(traceFrames, carId);
    if (benchFrames > 0) return runBench(benchFrames, carId);
    if (autoMode) return runAuto(shotDir, autoFrames, carId, shotEvery, place,
                                 placeX, placeZ, placeYaw);

    sf::RenderWindow window(sf::VideoMode({unsigned(kWindowWidth), unsigned(kWindowHeight)}),
                            "Redline Rumble 4 - Native");
    window.setVerticalSyncEnabled(true);

    Renderer r(kWidth, kHeight);
    World world;
    world.build();
    buildCarAssets();

    sf::Texture screenTex(sf::Vector2u(kWidth, kHeight));
    screenTex.setSmooth(true);
    sf::Sprite screenSprite(screenTex);
    screenSprite.setScale({float(kWindowWidth) / float(kWidth),
                           float(kWindowHeight) / float(kHeight)});

    Car car;
    car.setCar(carId);
    if (place) {
        car.pos = {placeX, 0.0f, placeZ};
        car.yaw = deg2rad(placeYaw);
        world.resolveCarCollision(car);
    }
    ChaseCam chase;
    chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);

    Audio audio;
    audio.load();
    audio.setCar(carId);
    audio.startMusic();

    sf::Clock clock;
    double accumulator = 0.0;
    float fps = 60.0f;
    float fpsTimer = 0.0f;
    int frameCount = 0;
    int shotNumber = 0;
    bool showDebug = false;
    const double step = 1.0 / 60.0;

    while (window.isOpen()) {
        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            } else if (ev->is<sf::Event::KeyPressed>()) {
                const auto *key = ev->getIf<sf::Event::KeyPressed>();
                if (key && key->code == sf::Keyboard::Key::Escape) window.close();
                if (key && key->code == sf::Keyboard::Key::M) audio.toggleMute();
                if (key && key->code == sf::Keyboard::Key::F1) showDebug = !showDebug;
                if (key && key->code == sf::Keyboard::Key::F12) {
                    char name[64];
                    std::snprintf(name, sizeof(name), "shot_%03d.png", ++shotNumber);
                    fs::path p = fs::path(shotDir) / name;
                    saveFrame(r, p);
                }
            }
        }

        CarInput in;
        in.up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
        in.down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
        in.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
        in.right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
        in.boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);

        sf::Time dt = clock.restart();
        accumulator += dt.asSeconds();
        int steps = 0;
        while (accumulator >= step && steps < 5) {
            // enterFrame order: doEngineSounds -> doControls/... -> doCam.
            audio.tickEngine(in.up);
            car.update(in, 1.0f);
            world.resolveCarCollision(car);
            audio.tickCar(car, in);
            chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
            accumulator -= step;
            ++steps;
        }
        if (accumulator > step * 5) accumulator = 0.0;

        fpsTimer += dt.asSeconds();
        ++frameCount;
        if (fpsTimer >= 0.5f) {
            fps = frameCount / fpsTimer;
            fpsTimer = 0.0f;
            frameCount = 0;
        }

        renderFrame(r, world, car, chase, fps, audio.muted());
        if (showDebug) drawDebugHud(r, world, car, fps);

        screenTex.update(reinterpret_cast<const std::uint8_t *>(r.color.data()));
        window.clear();
        window.draw(screenSprite);
        window.display();
    }
    return 0;
}
