#include "colortank.hpp"
#include "garage.hpp"
#include "assets.hpp"
#include "car.hpp"
#include "ferris.hpp"

#include <cmath>
#include <cstdio>

namespace rr4 {

namespace {

// mFerrisWheel.worldPosition + (-1152, -440, 1320).
const Vec3 kCansPos{FerrisWheel::kX - 1152.0f, FerrisWheel::kY - 440.0f, FerrisWheel::kZ + 1320.0f};
// The car parks this far short of the cans (z), once inside the bay.
constexpr float kParkDist = 160.0f;
constexpr float kBayHalfWidth = 100.0f, kBayDepth = 300.0f;
constexpr float kStraighten = 0.9f;       // rotation * 0.9 per frame in the bay
constexpr float kSwingDegPerFrame = 5.0f; // colorCalcAngle step, half a cosine to 180
constexpr float kSwingEnd = 180.0f;
constexpr float kCanTurnDeg = 45.0f;      // the cans turn 45 degrees per paint job
constexpr float kPourPerFrame = 0.01f;    // colorSplashPercent step
constexpr float kPaintLands = 0.5f;       // the paint reaches the car half way
constexpr float kCanHalfPoured = 0.5f;    // can textureTransform.position.y
constexpr float kPoured = 1.0f;           // splash fully poured
constexpr int kSplashPanel = 7;           // SPLASH OF COLOR MACHINE
constexpr int kHidePanel = 0;
constexpr float kWrecked = 1.0f;          // g_DamagePercent at which the car is wrecked
int gCansMesh = -1;
int gCanTex[kPaintJobs + 1], gSplashTex[kPaintJobs + 1];   // Textures 171.. / 161.., index 1..kPaintJobs
int gSplashMat = -1;             // TILE_FerrisWheelObjects_002 shaderList[13]

int loadHudTexture(const char *name, int n) {
    char path[64];
    std::snprintf(path, sizeof(path), "assets/hud/%s%d.png", name, n);
    Texture t;
    if (!loadTexture(path, t)) return -1;
    t.buildMips();
    gAssets.textures.push_back(std::move(t));
    return int(gAssets.textures.size()) - 1;
}

void setSplash(int texture, float v) {
    if (gSplashMat < 0) return;
    AssetMaterial &m = gAssets.materials[size_t(gSplashMat)];
    if (texture >= 0) m.texture = texture;
    m.dv = v;
}

} // namespace

void ColorTank::setup() {
    gCansMesh = gAssets.meshIndex("FerrisColorPaintCans_000");
    static bool loaded = false;   // a new session sets the machine up again
    if (!loaded) {
        for (int i = 1; i <= kPaintJobs; ++i) {
            gCanTex[i] = loadHudTexture("ColorCan", i);
            gSplashTex[i] = loadHudTexture("ColorSplash", i);
        }
        loaded = true;
    }
    for (size_t i = 0; i < gAssets.materials.size(); ++i)
        if (gAssets.materials[i].name == "Texture_FerrisColorSplash") gSplashMat = int(i);
    // The splash starts poured out, as it is after every pour.
    setSplash(-1, kPoured);
    can1_ = can2_ = 0;   // the cans keep their own textures until the first pour
}

// The car sits square under the cans, kParkDist short of them.
void ColorTank::lockCar(Car &car) const {
    car.pos.x = kCansPos.x;
    car.pos.z = kCansPos.z - kParkDist;
    car.setRotationDeg({});
    car.vel = {};
}

void ColorTank::pour(int newID, int prevID, bool right) {
    splash_ = 0.0f;
    angle_ = 0.0f;
    // RIGHT turns the cans from 0 on by kCanTurnDeg; LEFT starts them a
    // turn back and swings them home.
    rot_ = prevRot_ = right ? 0.0f : -kCanTurnDeg;
    targetRot_ = right ? kCanTurnDeg : -kCanTurnDeg;
    // The new paint goes on the can turning into place, the old one on the
    // can leaving, already half poured.
    if (right) { can1_ = newID; can1V_ = 0.0f; can2_ = prevID; can2V_ = kCanHalfPoured; }
    else       { can2_ = newID; can2V_ = 0.0f; can1_ = prevID; can1V_ = kCanHalfPoured; }
    setSplash(gSplashTex[newID], 0.0f);
    state_ = kSwing;
}

void ColorTank::update(Car &car, const CarInput &in, int carColors[kCarCount]) {
    switch (state_) {
    case kIdle: {
        Vec3 d = kCansPos - car.pos;
        if (std::fabs(d.x) < kBayHalfWidth && d.z > 0.0f && d.z < kBayDepth) {
            car.pos.x = kCansPos.x;
            // mPlayerCar.transform.rotation * 0.9
            car.setRotationDeg(car.rotationDeg() * kStraighten);
            if (d.z < kParkDist && car.damage < kWrecked) {
                car.pos.z = kCansPos.z - kParkDist;
                car.vel.z = 0.0f;
                state_ = kParked;
                car.doPanel(kSplashPanel, 0);
            }
        }
        break;
    }
    case kParked:
        car.pos.x = kCansPos.x;
        car.pos.z = kCansPos.z - kParkDist;
        car.setRotationDeg({});
        if (in.down) {
            state_ = kIdle;
            car.doPanel(kHidePanel, 0);
            car.saveGame = true;
        } else if (in.left || in.right) {
            car.doPanel(kHidePanel, 0);
            int prev = splashID_;
            splashID_ = in.left ? (splashID_ == 1 ? kPaintJobs : splashID_ - 1)
                                : (splashID_ == kPaintJobs ? 1 : splashID_ + 1);
            pour(splashID_, prev, !in.left);
        }
        break;
    case kSwing:   // half a cosine from 0 to kSwingEnd degrees
        lockCar(car);
        angle_ += kSwingDegPerFrame;
        if (angle_ >= kSwingEnd) {
            angle_ = kSwingEnd;
            state_ = kPour;
        }
        rot_ = prevRot_ + targetRot_ * ((std::cos(deg2rad(angle_)) - 1.0f) / 2.0f);
        break;
    case kPour: {
        lockCar(car);
        float was = splash_;
        splash_ += kPourPerFrame;
        if (splash_ >= kPoured) {
            splash_ = kPoured;
            state_ = kParked;
        }
        if (was < kPaintLands && splash_ >= kPaintLands) {
            carColors[car.carId - 1] = splashID_;
            setCarPaint(car.carId, splashID_);
        }
        can1V_ = can2V_ = splash_ * kCanHalfPoured;
        setSplash(-1, splash_);
        break;
    }
    }
}

void ColorTank::draw(Renderer &r, const Camera &cam) const {
    if (gCansMesh < 0) return;
    const AssetMesh &mesh = gAssets.meshes[size_t(gCansMesh)];
    Mat4 m = Mat4::translation(kCansPos) * Mat4::rotationY(deg2rad(rot_));
    r.setWorldFog();
    for (size_t gi = 0; gi < mesh.groups.size(); ++gi) {
        const AssetGroup &g = mesh.groups[gi];
        if (g.count == 0 || g.material < 0) continue;
        const AssetMaterial &mat = gAssets.materials[size_t(g.material)];
        // Mesh groups follow the shader list: group 0 is shaderList[1].
        const bool first = gi == 0;
        const int tex = first ? can1_ : can2_;
        const Texture *t = tex > 0 && gCanTex[tex] >= 0 ? &gAssets.textures[size_t(gCanTex[tex])]
                                                       : gAssets.texture(g.material);
        r.setSurface(mat.lighting);
        r.setUVOffset(0.0f, first ? can1V_ : can2V_);
        r.drawMesh(cam, m, mesh, t, mat.tint(), false, 1, mat.opacity, true, g.start, g.count);
    }
    r.setUVOffset(0.0f, 0.0f);
    r.resetSurface();
}

} // namespace rr4
