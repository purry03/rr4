#include "audio.hpp"
#include "car.hpp"
#include "font.hpp"
#include "hud.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "world.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include "cinematic.hpp"
#include "gl_renderer.hpp"
#include "gamepad.hpp"
#include "menu.hpp"
#include "savegame.hpp"
#include "settings.hpp"

#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace rr4;

namespace {

// The software renderer draws at the window resolution (no upscaling).
constexpr int kWidth = 960;
constexpr int kHeight = 600;

struct ChaseCam {
    Camera cam;
    Vec3 aim;
    float range = 200.0f;   // doSetupPlayerCar: camRange = 200
    float height = 50.0f;   // doSetupPlayerCar: camHeight = 50
    int state = 1;          // camState
    int lookingForwards = 0;
    bool init = false;

    // Free look (port addition, not in the original): the gamepad right
    // stick orbits the camera around the car. While orbiting, the original
    // follow-behind rotation is paused. After the stick has been idle for
    // kOrbitReturnDelay ticks of driving, the camera eases back behind the
    // car; R3 (recenter) snaps it back at once.
    static constexpr float kOrbitRateDeg = 3.0f;          // per tick at full deflection
    static constexpr int kOrbitReturnDelay = 180;         // 3 s of driving at 60 Hz
    static constexpr float kOrbitReturnSpeed = 5.0f;      // car speed that counts as driving
    static constexpr float kOrbitReturnEase = 0.04f;      // fraction of the angle per tick
    static constexpr float kOrbitReturnMinDeg = 0.5f;     // slowest return step
    static constexpr float kOrbitSnapDeg = 1.0f;          // close enough: hand back to doCam
    static constexpr float kOrbitRamp = 0.08f;            // fraction of the speed gap closed per tick (ramp up / down)
    static constexpr float kRecenterEase = 0.10f;         // R3 return: faster than the idle return
    static constexpr float kRecenterMinDeg = 1.5f;
    float orbitAxis = 0.0f;   // right stick X, -1..1 (right = counter-clockwise from above)
    bool recenter = false;    // R3 pressed
    bool recentering = false; // easing back after R3
    float orbitVel = 0.0f;    // smoothed orbit speed, deg per tick
    bool freeLook = false;
    int orbitIdle = 0;

    // doCam() from BehaviorScript 4 - PlayerCar.ls.
    void update(const Car &car, int tileID, float factor) {
        if (!init) {
            // doSetupPlayerCar: camAim and cam.worldPosition around the car.
            aim = car.pos + Vec3(-80, 50, 200);
            cam.pos = car.pos + Vec3(-80, 50, -200);
            init = true;
            return;
        }

        if (car.respawned) {
            // doDamageMaxOutReset: camState 1, camRange 200, camHeight 50.
            state = 1;
            freeLook = recentering = false;
            orbitVel = 0.0f;
            range = 200.0f;
            height = 50.0f;
            aim = car.pos + Vec3(0, 50, -100);
            cam.pos = car.pos + Vec3(0, 200, -300);
        }

        if (car.raceReturn) {
            freeLook = recentering = false;
            orbitVel = 0.0f;
            // doRaceFlag: camAim and cam.worldPosition as doSetupPlayerCar.
            aim = car.pos + Vec3(-80, 50, 200);
            cam.pos = car.pos + Vec3(-80, 50, -200);
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

        // Ramp the orbit speed up and down instead of jumping to the stick value.
        // Pushing right turns the camera counter-clockwise from above.
        orbitVel += (-orbitAxis * kOrbitRateDeg - orbitVel) * (1.0f - std::pow(1.0f - kOrbitRamp, factor));
        if (std::fabs(orbitVel) < 0.01f && orbitAxis == 0.0f) orbitVel = 0.0f;
        if (orbitAxis != 0.0f) { freeLook = true; orbitIdle = 0; recentering = false; }
        if (recenter && freeLook) recentering = true;
        if (freeLook) tempRot = 0.0f;

        Vec3 offset(cam.pos.x - car.pos.x, 0, cam.pos.z - car.pos.z);
        if (offset.length() > 1e-4f) {
            offset = offset.normalized();
            if (tempRot != 0.0f)
                offset = Mat4::rotationY(deg2rad(tempRot * factor)).transformVector(offset);
            if (orbitVel != 0.0f)
                offset = Mat4::rotationY(deg2rad(orbitVel * factor)).transformVector(offset);
            const bool idleReturn = freeLook && orbitAxis == 0.0f && car.vel.length() > kOrbitReturnSpeed &&
                                    ++orbitIdle > kOrbitReturnDelay;
            if (freeLook && (recentering || idleReturn)) {
                // Ease the camera direction back behind the car.
                float delta = std::atan2(front.x, front.z) - std::atan2(offset.x, offset.z);
                while (delta > PI) delta -= 2.0f * PI;
                while (delta < -PI) delta += 2.0f * PI;
                const float deg = delta * 180.0f / PI;
                if (std::fabs(deg) < kOrbitSnapDeg) {
                    freeLook = false;
                    recentering = false;
                    orbitVel = 0.0f;
                } else {
                    const float ease = recentering ? kRecenterEase : kOrbitReturnEase;
                    const float minDeg = recentering ? kRecenterMinDeg : kOrbitReturnMinDeg;
                    float step = deg * ease;
                    if (std::fabs(step) < minDeg) step = std::copysign(minDeg, deg);
                    offset = Mat4::rotationY(deg2rad(step * factor)).transformVector(offset);
                }
            }
        }
        recenter = false;

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
                  car.pitchDeg(), car.rollDeg(), car.boostMeter * 100.0f);
    drawText(r, 14, 118, buf, packRGBA(200, 214, 226), 1);
    drawText(r, 14, 134, "ARROWS DRIVE  SHIFT BOOST  M MUTE  F1 DEBUG",
             packRGBA(150, 170, 190), 1);
}

void renderFrame(Renderer &r, World &world, Car &car, ChaseCam &chase, float fps,
                 bool muted = false) {
    r.clearSky(chase.cam.pos);
    world.draw(r, chase.cam);
    car.draw(r, chase.cam);
    drawHud(r, car, muted, world, chase.cam);
    r.flush();
}

bool saveFrame(Renderer &r, const fs::path &path) {
    fs::create_directories(path.parent_path());
    static const bool imageReady = [] {
        IMG_Init(IMG_INIT_PNG);
        return true;
    }();
    (void)imageReady;
    std::vector<uint32_t> px;
    r.readPixels(px);
    // readPixels() is top row first, packed as R, G, B, A bytes, which is
    // SDL_PIXELFORMAT_ABGR8888 on little-endian targets.
    SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormatFrom(
        px.data(), r.width, r.height, 32, r.width * 4, SDL_PIXELFORMAT_ABGR8888);
    if (!surf) return false;
    const bool ok = IMG_SavePNG(surf, path.string().c_str()) == 0;
    SDL_FreeSurface(surf);
    return ok;
}

bool ensureVideo() {
    if (SDL_WasInit(SDL_INIT_VIDEO)) return true;
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "video: SDL init failed (%s)\n", SDL_GetError());
        return false;
    }
    return true;
}

bool displayModeSupported(int w, int h) {
    const int count = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < count; ++i) {
        SDL_DisplayMode mode{};
        if (SDL_GetDisplayMode(0, i, &mode) == 0 && mode.w == w && mode.h == h) return true;
    }
    return false;
}

// Window sizes to offer: the display's fullscreen modes (one per size, at
// least the original 800 x 480 stage), smallest first.
std::vector<Resolution> availableResolutions() {
    std::vector<Resolution> out;
    ensureVideo();
    auto add = [&](int w, int h) {
        if (w < Settings::kMinWidth || h < Settings::kMinHeight) return;
        if (std::none_of(out.begin(), out.end(),
                         [&](const Resolution &r) { return r.w == w && r.h == h; }))
            out.push_back({w, h});
    };
    const int count = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < count; ++i) {
        SDL_DisplayMode mode{};
        if (SDL_GetDisplayMode(0, i, &mode) == 0) add(mode.w, mode.h);
    }
    SDL_DisplayMode desktop{};
    if (SDL_GetDesktopDisplayMode(0, &desktop) == 0) add(desktop.w, desktop.h);
    std::sort(out.begin(), out.end(), [](const Resolution &a, const Resolution &b) {
        return a.w != b.w ? a.w < b.w : a.h < b.h;
    });
    if (out.empty()) out.push_back({kWidth, kHeight});
    return out;
}

// Settings that need a new window or renderer.
bool videoChanged(const Settings &a, const Settings &b) {
    return a.width != b.width || a.height != b.height || a.fullscreen != b.fullscreen ||
           a.vsync != b.vsync || a.msaa != b.msaa || a.shadows != b.shadows ||
           a.smoothTextures != b.smoothTextures;
}

// Renderer choice. The window uses OpenGL unless --soft is given; headless
// modes use the software rasterizer unless --gl is given, in which case an
// offscreen GL context is created.
struct RenderOptions {
    bool gl = false;
    GLSettings glSettings;
};
RenderOptions gRender;

void setGLAttributes() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

struct Display {
    SDL_Window *window = nullptr;
    SDL_GLContext gl = nullptr;
    SDL_Renderer *sdlRenderer = nullptr;   // software renderer presentation
    SDL_Texture *screenTexture = nullptr;

    explicit operator bool() const { return window != nullptr; }

    void destroy() {
        if (screenTexture) SDL_DestroyTexture(screenTexture);
        if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
        if (gl) SDL_GL_DeleteContext(gl);
        if (window) SDL_DestroyWindow(window);
        screenTexture = nullptr;
        sdlRenderer = nullptr;
        gl = nullptr;
        window = nullptr;
    }
};

struct Backend {
    SDL_Window *window = nullptr;      // headless GL only
    SDL_GLContext context = nullptr;
    std::unique_ptr<Renderer> renderer;

    Backend() = default;
    Backend(const Backend &) = delete;
    Backend &operator=(const Backend &) = delete;
    Backend(Backend &&other) noexcept
        : window(other.window), context(other.context), renderer(std::move(other.renderer)) {
        other.window = nullptr;
        other.context = nullptr;
    }
    Backend &operator=(Backend &&) = delete;

    ~Backend() {
        // GL objects go first, while their context is still current.
        renderer.reset();
        if (context) SDL_GL_DeleteContext(context);
        if (window) SDL_DestroyWindow(window);
    }
};

void createGameWindow(Display &display, const Settings &s, bool useGL) {
    display.destroy();
    ensureVideo();

    int w = s.width, h = s.height;
    // A fullscreen size the display cannot do falls back to the desktop.
    if (s.fullscreen && !displayModeSupported(w, h)) {
        SDL_DisplayMode desktop{};
        if (SDL_GetDesktopDisplayMode(0, &desktop) == 0) {
            w = desktop.w;
            h = desktop.h;
        }
    }

    Uint32 flags = SDL_WINDOW_SHOWN;
    if (s.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (useGL) {
        setGLAttributes();
        flags |= SDL_WINDOW_OPENGL;
    }
    display.window = SDL_CreateWindow("Redline Rumble 4", SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, w, h, flags);
    if (!display.window) {
        std::fprintf(stderr, "video: cannot create window (%s)\n", SDL_GetError());
        return;
    }

    if (useGL) {
        display.gl = SDL_GL_CreateContext(display.window);
        if (!display.gl) {
            std::fprintf(stderr, "gl: cannot create context (%s)\n", SDL_GetError());
            display.destroy();
            return;
        }
        SDL_GL_MakeCurrent(display.window, display.gl);
        SDL_GL_SetSwapInterval(s.vsync ? 1 : 0);
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, s.smoothTextures ? "linear" : "nearest");
    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    if (s.vsync) rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    display.sdlRenderer = SDL_CreateRenderer(display.window, -1, rendererFlags);
    if (!display.sdlRenderer)
        display.sdlRenderer = SDL_CreateRenderer(display.window, -1, SDL_RENDERER_SOFTWARE);
    if (!display.sdlRenderer) {
        std::fprintf(stderr, "video: cannot create renderer (%s)\n", SDL_GetError());
        display.destroy();
        return;
    }
    display.screenTexture = SDL_CreateTexture(display.sdlRenderer, SDL_PIXELFORMAT_ABGR8888,
                                              SDL_TEXTUREACCESS_STREAMING, s.width, s.height);
    if (!display.screenTexture) {
        std::fprintf(stderr, "video: cannot create screen texture (%s)\n", SDL_GetError());
        display.destroy();
        return;
    }
    SDL_SetTextureBlendMode(display.screenTexture, SDL_BLENDMODE_NONE);
}

Backend makeHeadlessBackend() {
    Backend b;
    if (gRender.gl) {
        if (!ensureVideo()) return b;
        setGLAttributes();
        b.window = SDL_CreateWindow("Redline Rumble 4 (headless)", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        if (!b.window) {
            std::fprintf(stderr, "gl: cannot create headless window (%s)\n", SDL_GetError());
            return b;
        }
        b.context = SDL_GL_CreateContext(b.window);
        if (!b.context) {
            std::fprintf(stderr, "gl: cannot create headless context (%s)\n", SDL_GetError());
            return b;
        }
        SDL_GL_MakeCurrent(b.window, b.context);
        b.renderer = std::make_unique<GLRenderer>(kWidth, kHeight, gRender.glSettings);
    } else {
        b.renderer = std::make_unique<SoftRenderer>(kWidth, kHeight);
    }
    return b;
}

// Waypoint autopilot used only for headless verification: it follows the
// original road loop so screenshots prove the car can drive the whole map.
struct Waypoint { float x, z; };

const Waypoint kWaypoints[] = {
    {36000, 26500}, {54500, 28500}, {57500, 26500}, {57500, 12500},
    {53500, 10500}, {26500, 12500}, {18500, 10500}, {17500, 16500},
    {17500, 46500}, {32500, 47500}, {33500, 44500}, {34500, 35500},
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
    float err = wrapAngle(desired - car.heading());
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
    Backend backend = makeHeadlessBackend();
    Renderer &r = *backend.renderer;
    World world;
    world.build();
    buildCarAssets();

    Car car;
    car.setCar(carId);
    if (place) {
        car.pos = {px, 0.0f, pz};
        car.setRotationDeg({0.0f, pyaw, 0.0f});
        world.resolveCarCollision(car);
        world.updateGround(car);
    }
    ChaseCam chase;
    chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);

    fs::create_directories(shotDir);
    int saved = 0;
    int wpIndex = 0;
    int stuckFrames = 0;

    std::printf("auto-drive: %d frames at 60 Hz\n", totalFrames);
    std::printf("start pos=(%.2f, %.2f, %.2f) yaw=%.1f deg tile=%d (%s)\n",
                car.pos.x, car.pos.y, car.pos.z, car.heading() * 180.0f / PI,
                world.tileIDAt(car.pos.x, car.pos.z),
                tileName(world.tileIDAt(car.pos.x, car.pos.z)));

    float maxSpeed = 0.0f;
    float minX = 1e9f, maxX = -1e9f, minZ = 1e9f, maxZ = -1e9f;

    for (int frame = 0; frame <= totalFrames; ++frame) {
        CarInput in = autoInput(car, wpIndex, frame, stuckFrames);
        car.update(in, 1.0f, [&](Car &c) { world.updateGround(c); },
                   [&](Car &c) { world.doCollisions(c); world.doWorldObjects(c, in); });
        tickHud(car);
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);

        if (frame % 60 == 0) {
            std::printf("frame %4d  pos=(%8.1f,%8.1f) yaw=%6.1f  speed=%5.1f km/h  "
                        "tilt=%+5.1f/%+5.1f  damage=%.2f  tile=%02d %s\n",
                        frame, car.pos.x, car.pos.z, car.heading() * 180.0f / PI,
                        car.speedKmh, car.pitchDeg(), car.rollDeg(), car.damage,
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
                    f, car.throttle, car.steer, car.heading() * 180.0f / PI, car.yawVelDeg,
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
                "range,height,aimX,aimY,aimZ,camX,camY,camZ,"
                "zAxisX,zAxisY,zAxisZ,xAxisX,xAxisY,xAxisZ\n");
    for (int f = 0; f <= frames; ++f) {
        CarInput in = autoInput(car, wpIndex, f, stuckFrames);
        car.update(in, 1.0f, [&](Car &c) { world.updateGround(c); },
                   [&](Car &c) { world.doCollisions(c); world.doWorldObjects(c, in); });
        tickHud(car);
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
        const Vec3 zAxis = car.forward(), xAxis = car.rightAxis();
        std::printf("%d,%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"
                    "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"
                    "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                    f, world.tileIDAt(car.pos.x, car.pos.z), car.throttle,
                    car.heading() * 180.0f / PI, car.pos.x, car.pos.y, car.pos.z,
                    car.vel.x, car.vel.y, car.vel.z, chase.range, chase.height,
                    chase.aim.x, chase.aim.y, chase.aim.z, chase.cam.pos.x,
                    chase.cam.pos.y, chase.cam.pos.z, zAxis.x, zAxis.y, zAxis.z,
                    xAxis.x, xAxis.y, xAxis.z);
    }
    return 0;
}

// Development aid: run the main menu over the cinematic city headless and
// save a frame every `every` ticks, to check the shots.
int runMenuPreview(const std::string &shotDir, int frames, int every) {
    Backend backend = makeHeadlessBackend();
    Renderer &r = *backend.renderer;
    World world;
    world.build();
    buildCarAssets();
    SaveGame save;
    world.newSession(save.carsOwned, save.carColors);
    MainMenu menu;
    menu.load();
    menu.open(false);
    CinematicCamera cinematic;
    menu.setSettings(Settings{}, availableResolutions());
    // Halfway through, step down to SETTINGS and open it.
    const int settingsAt = frames / 2;
    for (int f = 0; f < frames; ++f) {
        cinematic.update(world);
        MenuInput in;
        in.down = f == settingsAt || f == settingsAt + 1;
        in.confirm = f == settingsAt + 2;
        menu.update(in);
        if (f % every != 0) continue;
        r.clearSky(cinematic.camera().pos);
        world.draw(r, cinematic.camera());
        menu.draw(r, cinematic.fade());
        r.flush();
        char name[64];
        std::snprintf(name, sizeof(name), "menu_%05d.png", f);
        saveFrame(r, fs::path(shotDir) / name);
    }
    return 0;
}

int runBench(int frames, int carId) {
    Backend backend = makeHeadlessBackend();
    Renderer &r = *backend.renderer;
    World world;
    world.build();
    buildCarAssets();

    Car car;
    car.setCar(carId);
    ChaseCam chase;
    chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
    int wpIndex = 0, stuckFrames = 0;
    std::vector<uint32_t> frame;

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < frames; ++i) {
        CarInput in = autoInput(car, wpIndex, i, stuckFrames);
        car.update(in, 1.0f, [&](Car &c) { world.updateGround(c); },
                   [&](Car &c) { world.doCollisions(c); world.doWorldObjects(c, in); });
        tickHud(car);
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
        renderFrame(r, world, car, chase, 60.0f);
        // GL only queues work; reading the frame back waits for the GPU.
        if (gRender.gl) r.readPixels(frame);
    }
    auto t1 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count() / frames;
    std::printf("bench: %d frames, %.2f ms/frame, %.1f fps (%s, %dx%d)\n", frames, ms, 1000.0 / ms,
                gRender.gl ? "OpenGL, with readback" : "software rasterizer", kWidth, kHeight);
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
        SDL_Delay(10);
    }
    in = {};
    in.down = true;
    audio.tickEngine(false);
    car.update(in, 1.0f);
    audio.tickCar(car, in);
    SDL_Delay(150);
    in = {};
    in.boost = true;
    audio.tickEngine(false);
    car.update(in, 1.0f);
    audio.tickCar(car, in);
    SDL_Delay(150);
    audio.toggleMute();
    audio.toggleMute();
    std::printf("audio-test: engine/music/brake/nitro/mute path complete\n");
    return 0;
}

// Development aid: hold the throttle from a placed position so tile collision
// pushes can be observed without the waypoint autopilot steering away.
int runDrive(int frames, float x, float z, float yawDeg, int carId) {
    World world;
    world.build();
    buildCarAssets();
    Car car;
    car.setCar(carId);
    car.pos = {x, 0.0f, z};
    car.setRotationDeg({0.0f, yawDeg, 0.0f});
    world.resolveCarCollision(car);
    world.updateGround(car);
    ChaseCam chase;
    chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
    std::printf("frame,posX,posY,posZ,yawDeg,speed,damage,impact,rotX,rotZ,tile\n");
    for (int f = 1; f <= frames; ++f) {
        CarInput in;
        in.up = true;
        car.update(in, 1.0f, [&](Car &c) { world.updateGround(c); },
                   [&](Car &c) { world.doCollisions(c); world.doWorldObjects(c, in); });
        tickHud(car);
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
        if (f % 5 == 0 || f == frames)
            std::printf("%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.4f,%.2f,%.2f,%.2f,%d\n", f,
                        car.pos.x, car.pos.y, car.pos.z,
                        car.heading() * 180.0f / PI, car.speedKmh, car.damage,
                        car.impactSpeed, car.rotXDeg, car.rotZDeg,
                        world.tileIDAt(car.pos.x, car.pos.z));
    }
    return 0;
}

// Development aid: report the settled attitude at one spot, so the suspension
// and the pitch/roll signs can be checked against a known bank.
int runProbe(float x, float z, float yawDeg, int carId) {
    World world;
    world.build();
    Car car;
    car.setCar(carId);
    car.pos = {x, 0.0f, z};
    car.setRotationDeg({0.0f, yawDeg, 0.0f});
    world.resolveCarCollision(car);
    // Let doRayCasting settle the chassis the same way a real step does.
    for (int i = 0; i < 240; ++i) {
        CarInput in;
        car.update(in, 1.0f, [&](Car &c) { world.updateGround(c); },
                   [&](Car &c) { world.doCollisions(c); world.doWorldObjects(c, in); });
    }
    int id = world.tileIDAt(car.pos.x, car.pos.z);
    std::printf("probe x=%.1f z=%.1f yaw=%.1f car=%d tile=%02d %s y=%.2f "
                "pitch=%+.2f roll=%+.2f ground=%+.2f/%+.2f\n",
                car.pos.x, car.pos.z, yawDeg, carId, id, tileName(id),
                car.pos.y, car.pitchDeg(), car.rollDeg(),
                car.groundPitchDeg, car.groundRollDeg);
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
            car.setRotationDeg({});
            car.vel = {};
            car.rotXDeg = car.rotZDeg = 0.0f;
            car.groundPitchDeg = car.groundRollDeg = 0.0f;
            world.updateGround(car);
            float score = std::max(std::fabs(car.groundPitchDeg),
                                   std::fabs(car.groundRollDeg));
            if (score > 1.0f && score < 4.0f) hits.push_back({float(x), float(z), car.pos.y,
                                              car.groundPitchDeg, car.groundRollDeg, id});
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
    bool carGiven = false;
    int driveFrames = 0;
    bool soft = false;
    int menuPreview = 0;
    bool cliNoShadows = false;
    int cliMsaa = -1;

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
            carGiven = true;
            carId = std::atoi(argv[++i]);
        } else if (a == "--shot-every" && i + 1 < argc) {
            shotEvery = std::atoi(argv[++i]);
        } else if (a == "--drive" && i + 1 < argc) {
            driveFrames = std::atoi(argv[++i]);
        } else if (a == "--slope-scan") {
            slopeScan = true;
        } else if (a == "--place" && i + 3 < argc) {
            place = true;
            placeX = float(std::atof(argv[++i]));
            placeZ = float(std::atof(argv[++i]));
            placeYaw = float(std::atof(argv[++i]));
        } else if (a == "--menu-preview" && i + 1 < argc) {
            menuPreview = std::atoi(argv[++i]);
        } else if (a == "--soft") {
            soft = true;
        } else if (a == "--gl") {
            gRender.gl = true;
        } else if (a == "--no-shadows") {
            gRender.glSettings.shadows = false;
            cliNoShadows = true;
        } else if (a == "--msaa" && i + 1 < argc) {
            gRender.glSettings.msaa = std::max(0, std::atoi(argv[++i]));
            cliMsaa = gRender.glSettings.msaa;
        } else if (a == "--probe" && i + 3 < argc) {
            probe = true;
            probeX = float(std::atof(argv[++i]));
            probeZ = float(std::atof(argv[++i]));
            probeYaw = float(std::atof(argv[++i]));
        }
    }

    if (driveFrames > 0) return runDrive(driveFrames, placeX, placeZ, placeYaw, carId);
    if (slopeScan) return runSlopeScan();
    if (probe) return runProbe(probeX, probeZ, probeYaw, carId);
    if (audioTest) return runAudioTest(carId);
    if (camTraceFrames >= 0) return runCamTrace(camTraceFrames, carId);
    if (traceFrames >= 0) return runTrace(traceFrames, carId);
    if (benchFrames > 0) return runBench(benchFrames, carId);
    if (menuPreview > 0) return runMenuPreview(shotDir, menuPreview, shotEvery);
    if (autoMode) return runAuto(shotDir, autoFrames, carId, shotEvery, place,
                                 placeX, placeZ, placeYaw);

    // rr4settings.txt; command-line options override it for this run.
    const std::filesystem::path settingsPath = defaultSettingsPath();
    Settings settings;
    if (settings.load(settingsPath)) std::printf("settings: loaded %s\n", settingsPath.string().c_str());
    if (std::getenv("RR4_NEAREST")) settings.smoothTextures = false;
    if (cliNoShadows) settings.shadows = false;
    if (cliMsaa >= 0) settings.msaa = cliMsaa;
    const std::vector<Resolution> resolutions = availableResolutions();

    const bool useGL = !soft;
    Display display;
    std::unique_ptr<Renderer> renderer;
    Settings applied;                         // what the window/renderer were built with
    // Window and renderer for the video settings. The renderer (and its GL
    // objects) goes first, while its context is still current.
    auto buildDisplay = [&] {
        renderer.reset();
        createGameWindow(display, settings, useGL);
        gSmoothTextures = settings.smoothTextures;
        if (!display) return;
        if (useGL) {
            GLSettings gs = gRender.glSettings;
            gs.msaa = settings.msaa;
            gs.shadows = settings.shadows;
            renderer = std::make_unique<GLRenderer>(settings.width, settings.height, gs);
        } else {
            renderer = std::make_unique<SoftRenderer>(settings.width, settings.height);
        }
        applied = settings;
    };
    buildDisplay();

    World world;
    world.build();
    buildCarAssets();
    world.drawDistance = settings.drawDistance;

    // Initialization: getPref(g_SaveGameName), or the default save. --car
    // overrides the driven car for this run.
    const std::filesystem::path savePath = defaultSavePath();
    SaveGame save;
    if (save.load(savePath)) std::printf("save: loaded %s\n", savePath.string().c_str());
    if (carGiven) {
        for (int &o : save.carsOwned) if (o == kOwnedDriving) o = kOwnedParked;
        save.carsOwned[carId - 1] = kOwnedDriving;
    }
    world.newSession(save.carsOwned, save.carColors);

    Audio audio;
    audio.load();
    audio.setVolumes(float(settings.musicVolume), float(settings.sfxVolume));
    audio.startMusic();

    Car car;
    ChaseCam chase;
    // doSetupPlayerCar for the saved driven car, at the original spawn.
    auto startSession = [&] {
        carId = save.playerCar();
        car = Car{};
        car.setCar(carId);
        car.damage = save.carDamage[carId - 1];
        car.boostMeter = save.carIgNitro[carId - 1];
        if (place) {
            car.pos = {placeX, 0.0f, placeZ};
            car.setRotationDeg({0.0f, placeYaw, 0.0f});
            world.resolveCarCollision(car);
            world.updateGround(car);
        }
        chase = ChaseCam{};
        chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
        resetHud();
        audio.setCar(carId);
    };
    // doSaveGameData: the lists plus the driven car's damage and igNitro.
    auto autosave = [&] {
        for (int i = 0; i < kCarCount; ++i) {
            save.carsOwned[i] = world.carsOwned[i];
            save.carColors[i] = world.carColors[i];
        }
        save.carDamage[car.carId - 1] = std::min(car.damage, SaveGame::kFull);
        save.carIgNitro[car.carId - 1] = car.boostMeter;
        save.save(savePath);
    };

    // The game opens on the main menu over the cinematic city (--place jumps
    // straight into driving, for testing spots).
    enum class Screen { Menu, Game };
    Screen screen = Screen::Menu;
    bool sessionActive = false;
    MainMenu menu;
    if (!menu.load()) std::fprintf(stderr, "menu: missing sprites, run tools/render_menu.py\n");
    menu.open(false);
    menu.setSettings(settings, resolutions);
    CinematicCamera cinematic;
    MenuInput menuIn;
    Gamepad pad;
    pad.init();
    if (place) {
        startSession();
        sessionActive = true;
        screen = Screen::Game;
    }

    auto previous = std::chrono::steady_clock::now();
    double accumulator = 0.0;
    float fps = 60.0f;
    float fpsTimer = 0.0f;
    int frameCount = 0;
    int shotNumber = 0;
    bool showDebug = false;
    const double step = 1.0 / 60.0;
    bool running = display && renderer;

    while (running) {
        Renderer &r = *renderer;
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            pad.handleEvent(ev);
            if (ev.type == SDL_QUIT) {
                if (sessionActive) autosave();
                running = false;
            } else if (ev.type == SDL_KEYDOWN && !ev.key.repeat) {
                const SDL_Keycode key = ev.key.keysym.sym;
                if (key == SDLK_m) audio.toggleMute();
                if (key == SDLK_F1) showDebug = !showDebug;
                if (key == SDLK_F12) {
                    char name[64];
                    std::snprintf(name, sizeof(name), "shot_%03d.png", ++shotNumber);
                    saveFrame(r, fs::path(shotDir) / name);
                }
                if (screen == Screen::Menu) {
                    menuIn.up |= key == SDLK_UP || key == SDLK_w;
                    menuIn.down |= key == SDLK_DOWN || key == SDLK_s;
                    menuIn.left |= key == SDLK_LEFT || key == SDLK_a;
                    menuIn.right |= key == SDLK_RIGHT || key == SDLK_d;
                    menuIn.confirm |= key == SDLK_RETURN || key == SDLK_KP_ENTER || key == SDLK_SPACE;
                    menuIn.back |= key == SDLK_ESCAPE;
                } else if (key == SDLK_ESCAPE) {
                    // Pause to the menu; the session resumes from RESUME.
                    autosave();
                    audio.stopAll();
                    menu.open(true);
                    screen = Screen::Menu;
                }
            }
        }
        if (!running) break;

        // Gamepad: menu presses feed the menu; Start pauses a running game.
        const MenuInput padMenu = pad.takeMenu();
        const bool padPause = pad.takePause();
        if (screen == Screen::Menu) {
            menuIn.up |= padMenu.up;
            menuIn.down |= padMenu.down;
            menuIn.left |= padMenu.left;
            menuIn.right |= padMenu.right;
            menuIn.confirm |= padMenu.confirm;
            menuIn.back |= padMenu.back;
        } else if (padPause) {
            autosave();
            audio.stopAll();
            menu.open(true);
            screen = Screen::Menu;
        }

        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        CarInput in;
        in.up = keys[SDL_SCANCODE_UP] != 0;
        in.down = keys[SDL_SCANCODE_DOWN] != 0;
        in.left = keys[SDL_SCANCODE_LEFT] != 0;
        in.right = keys[SDL_SCANCODE_RIGHT] != 0;
        in.boost = keys[SDL_SCANCODE_LSHIFT] != 0;
        in.handbrake = keys[SDL_SCANCODE_SPACE] != 0;
        const CarInput padIn = pad.car();
        in.up |= padIn.up;
        in.down |= padIn.down;
        in.left |= padIn.left;
        in.right |= padIn.right;
        in.boost |= padIn.boost;
        in.handbrake |= padIn.handbrake;
        chase.orbitAxis = screen == Screen::Game ? pad.cameraAxis() : 0.0f;
        if (pad.takeCameraReset()) chase.recenter = screen == Screen::Game;
        // Analog axes only apply when the keyboard is not steering / driving.
        if (!keys[SDL_SCANCODE_LEFT] && !keys[SDL_SCANCODE_RIGHT]) in.steerAxis = padIn.steerAxis;
        if (!keys[SDL_SCANCODE_UP] && !keys[SDL_SCANCODE_DOWN]) in.throttleAxis = padIn.throttleAxis;

        const auto now = std::chrono::steady_clock::now();
        const double dt = std::chrono::duration<double>(now - previous).count();
        previous = now;
        accumulator += dt;
        int steps = 0;
        while (accumulator >= step && steps < 5 && running) {
            if (screen == Screen::Menu) {
                cinematic.update(world);
                const MainMenu::Action action = menu.update(menuIn);
                menuIn = {};
                if (action == MainMenu::Action::NewGame) {
                    save = SaveGame{};
                    save.save(savePath);
                    world.newSession(save.carsOwned, save.carColors);
                    startSession();
                    sessionActive = true;
                    screen = Screen::Game;
                } else if (action == MainMenu::Action::Play) {
                    if (!sessionActive) startSession();
                    sessionActive = true;
                    screen = Screen::Game;
                } else if (action == MainMenu::Action::SettingsChanged) {
                    // Sound and draw distance apply at once; the window and
                    // renderer are rebuilt when the settings screen closes.
                    settings = menu.settings();
                    audio.setVolumes(float(settings.musicVolume), float(settings.sfxVolume));
                    world.drawDistance = settings.drawDistance;
                } else if (action == MainMenu::Action::SettingsClosed) {
                    settings = menu.settings();
                    settings.save(settingsPath);
                    if (videoChanged(applied, settings)) buildDisplay();
                    if (!display || !renderer) running = false;
                } else if (action == MainMenu::Action::Quit) {
                    if (sessionActive) autosave();
                    running = false;
                }
            } else {
                // enterFrame order: doEngineSounds -> doControls/... -> doCam.
                audio.tickEngine(in.up);
                car.update(in, 1.0f, [&](Car &c) { world.updateGround(c); },
                           [&](Car &c) { world.doCollisions(c); world.doWorldObjects(c, in); });
                audio.tickCar(car, in);
                pad.rumble(car, in);
                tickHud(car);
                chase.update(car, world.tileIDAt(car.pos.x, car.pos.z), 1.0f);
                if (car.saveGame) autosave();
            }
            accumulator -= step;
            ++steps;
        }
        if (accumulator > step * 5) accumulator = 0.0;
        if (!running) break;

        fpsTimer += float(dt);
        ++frameCount;
        if (fpsTimer >= 0.5f) {
            fps = frameCount / fpsTimer;
            fpsTimer = 0.0f;
            frameCount = 0;
        }

        // A settings change may have rebuilt the renderer this frame.
        Renderer &draw = *renderer;
        if (screen == Screen::Menu) {
            draw.clearSky(cinematic.camera().pos);
            world.draw(draw, cinematic.camera());
            menu.draw(draw, cinematic.fade());
            draw.flush();
        } else {
            renderFrame(draw, world, car, chase, fps, audio.muted());
            if (showDebug) drawDebugHud(draw, world, car, fps);
        }

        if (useGL) {
            draw.flush();
            int drawableW = 0, drawableH = 0;
            SDL_GL_GetDrawableSize(display.window, &drawableW, &drawableH);
            static_cast<GLRenderer &>(draw).present(drawableW, drawableH);
            SDL_GL_SwapWindow(display.window);
        } else {
            auto &soft = static_cast<SoftRenderer &>(draw);
            soft.flush();
            SDL_UpdateTexture(display.screenTexture, nullptr, soft.color.data(),
                              soft.width * int(sizeof(uint32_t)));
            SDL_RenderClear(display.sdlRenderer);
            SDL_RenderCopy(display.sdlRenderer, display.screenTexture, nullptr, nullptr);
            SDL_RenderPresent(display.sdlRenderer);
        }
    }
    return 0;
}
