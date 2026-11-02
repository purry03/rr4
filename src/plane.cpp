#include "plane.hpp"
#include "assets.hpp"

namespace rr4 {

namespace {
const Vec3 kAirPortOrigin{7500.0f, 0.0f, 7500.0f};   // airPortOrigin
} // namespace

void Plane::setup() {
    mesh_ = gAssets.meshIndex("TrafficBoeing747_001");
    pos_ = kAirPortOrigin;
    state_ = 7;
    speed_ = tilt_ = bank_ = yRot_ = groupYaw_ = 0.0f;
    startTicks_ = 0;
}

void Plane::startApproach() {
    pos_ = kAirPortOrigin + Vec3(0, 1000, 70000);
    tilt_ = bank_ = 0.0f;
    speed_ = 60.0f;
    yRot_ = 180.0f;
    groupYaw_ = yRot_;
    state_ = 8;
}

void Plane::translate(float x, float y, float z) {
    pos_ += Mat4::rotationY(deg2rad(groupYaw_)).transformVector({x, y, z});
}

Mat4 Plane::transform() const {
    // mPlane.transform.rotation = (-planeTilt, 0, -planeBank): X then Z.
    return Mat4::translation(pos_) * Mat4::rotationY(deg2rad(groupYaw_)) *
           Mat4::rotationZ(deg2rad(-bank_)) * Mat4::rotationX(deg2rad(-tilt_));
}

void Plane::update(long ticks) {
    const Vec3 diff = pos_ - kAirPortOrigin;
    switch (state_) {
    case 1:   // take-off roll
        if (diff.z > 20000.0f) ++state_;
        speed_ += 0.05f;
        translate(0, 0, speed_);
        break;
    case 2:   // rotate
        if (diff.z > 30000.0f) ++state_;
        if (tilt_ < 8.0f) tilt_ += 0.05f;
        speed_ += 0.05f;
        translate(0, 0, speed_);
        break;
    case 3:   // climb
        if (diff.z > 40000.0f) ++state_;
        if (tilt_ < 20.0f) tilt_ += 0.05f;
        speed_ += 0.05f;
        translate(0, 2, speed_);
        break;
    case 4:   // climb and bank
        if (diff.z > 50000.0f) ++state_;
        if (tilt_ < 20.0f) tilt_ += 0.05f;
        if (bank_ < 30.0f) bank_ += 0.05f;
        translate(0, 10, speed_);
        break;
    case 5:   // turn east
        if (tilt_ > 0.0f) tilt_ -= 0.05f;
        speed_ += 0.05f;
        yRot_ += 0.25f;
        translate(0, 10, speed_);
        if (bank_ < 30.0f) bank_ += 0.1f;
        groupYaw_ = yRot_;
        if (yRot_ >= 90.0f) {
            ++state_;
            yRot_ = 90.0f;
        }
        break;
    case 6:   // level out and fly off the map
        if (tilt_ > 0.0f) tilt_ -= 0.05f;
        else if (tilt_ < 0.0f) tilt_ = 0.0f;
        if (bank_ > 0.0f) bank_ -= 0.5f;
        speed_ += 0.05f;
        groupYaw_ = yRot_;
        translate(0, 0, speed_);
        if (diff.x > 80000.0f) ++state_;
        break;
    case 7:   // reappear on approach from the north
        startApproach();
        break;
    case 8:   // approach
        if (tilt_ < 10.0f) tilt_ += 0.05f;
        translate(0, 0, speed_);
        groupYaw_ = yRot_;
        if (pos_.z < 50000.0f) ++state_;
        break;
    case 9:   // descend
        if (tilt_ < 10.0f) tilt_ += 0.05f;
        speed_ -= 0.05f;
        translate(0, -5, speed_);
        groupYaw_ = yRot_;
        if (pos_.y <= 0.0f) {
            pos_.y = 0.0f;
            ++state_;
        }
        break;
    case 10:  // flare
        if (tilt_ > 0.0f) tilt_ -= 0.05f;
        speed_ -= 0.05f;
        translate(0, 0, speed_);
        groupYaw_ = yRot_;
        if (tilt_ <= 0.0f) {
            tilt_ = 0.0f;
            ++state_;
        }
        break;
    case 11:  // brake down the runway
        if (tilt_ > 0.0f) tilt_ -= 0.05f;
        if (speed_ > 2.0f) speed_ -= 0.028f;
        else speed_ = 2.0f;
        translate(0, 0, speed_);
        groupYaw_ = yRot_;
        if (pos_.z < 6600.0f) {
            ++state_;
            speed_ = 5.0f;
        }
        break;
    case 12:  // turn onto the taxiway
        if (tilt_ > 0.0f) tilt_ -= 0.05f;
        if (speed_ > 4.0f) speed_ -= 0.025f;
        else speed_ = 4.0f;
        yRot_ -= 0.25f;
        translate(0, 0, speed_);
        groupYaw_ = yRot_;
        if (yRot_ <= 90.0f) {
            yRot_ = 90.0f;
            speed_ = 4.0f;
            ++state_;
            startTicks_ = ticks;
        }
        break;
    case 13:  // taxi to the stand
        if (tilt_ > 0.0f) tilt_ -= 0.05f;
        translate(0, 0, speed_);
        groupYaw_ = yRot_;
        if (pos_.x > 9850.0f) {
            speed_ = 0.0f;
            ++state_;
            startTicks_ = ticks;
        }
        break;
    case 14:  // wait
        if (ticks - startTicks_ > 200) {
            speed_ = -4.0f;
            ++state_;
        }
        break;
    case 15:  // push back
        translate(0, 0, speed_);
        groupYaw_ = yRot_;
        if (pos_.x <= 8425.0f) {
            speed_ = -4.0f;
            ++state_;
            startTicks_ = ticks;
        }
        break;
    case 16:  // reverse round to face the runway
        yRot_ -= 0.25f;
        if (yRot_ <= 0.0f) {
            yRot_ = 0.0f;
            speed_ = 0.0f;
            state_ = 1;
            startTicks_ = ticks;
        }
        translate(0, 0, speed_);
        groupYaw_ = yRot_;
        break;
    default:
        break;
    }
}

void Plane::draw(Renderer &r, const Camera &cam) const {
    if (mesh_ >= 0) drawAssetMesh(r, cam, transform(), gAssets.meshes[size_t(mesh_)]);
}

} // namespace rr4
