#pragma once
// Minimal vector / matrix math for the Redline Rumble 4 native port.
// Coordinate convention used by the port: +Y is up, the car faces +Z when
// yaw == 0 (the original game uses the opposite Z sign; we keep all of the
// original physics constants and mirror the two axes that need it).

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace rr4 {

constexpr float PI = 3.14159265358979323846f;
inline float deg2rad(float d) { return d * (PI / 180.0f); }
inline float clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

struct Vec3 {
    float x = 0, y = 0, z = 0;
    Vec3() = default;
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vec3 operator+(const Vec3 &o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3 &o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator-() const { return {-x, -y, -z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const { return {x / s, y / s, z / s}; }
    Vec3 &operator+=(const Vec3 &o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3 &operator-=(const Vec3 &o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3 &operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    float dot(const Vec3 &o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3 &o) const {
        return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
    }
    float length() const { return std::sqrt(dot(*this)); }
    float lengthSq() const { return dot(*this); }
    Vec3 normalized() const {
        float l = length();
        return (l > 1e-6f) ? (*this / l) : Vec3{};
    }
    void normalize() { *this = normalized(); }
};

inline Vec3 lerp(const Vec3 &a, const Vec3 &b, float t) { return a + (b - a) * t; }
// Same byte layout as rr4::packRGBA: little-endian r, g, b, a in memory.
inline uint32_t packRGB(int r, int g, int b, int a = 255) {
    return uint32_t(clampf(float(r), 0, 255)) | (uint32_t(clampf(float(g), 0, 255)) << 8) |
           (uint32_t(clampf(float(b), 0, 255)) << 16) | (uint32_t(a) << 24);
}
inline void unpackRGB(uint32_t c, int &r, int &g, int &b) {
    r = c & 255; g = (c >> 8) & 255; b = (c >> 16) & 255;
}
inline uint32_t mixColor(uint32_t a, uint32_t b, float t) {
    int ar, ag, ab, br, bg, bb;
    unpackRGB(a, ar, ag, ab); unpackRGB(b, br, bg, bb);
    int aa = (a >> 24) & 255, ba = (b >> 24) & 255;
    return packRGB(int(ar + (br - ar) * t), int(ag + (bg - ag) * t),
                   int(ab + (bb - ab) * t), int(aa + (ba - aa) * t));
}

struct Mat4 {
    // Column-major, m[col*4 + row]. Matches OpenGL layout.
    float m[16]{};

    static Mat4 identity() {
        Mat4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }
    static Mat4 translation(const Vec3 &t) {
        Mat4 r = identity();
        r.m[12] = t.x; r.m[13] = t.y; r.m[14] = t.z;
        return r;
    }
    static Mat4 scale(const Vec3 &s) {
        Mat4 r = identity();
        r.m[0] = s.x; r.m[5] = s.y; r.m[10] = s.z;
        return r;
    }
    static Mat4 rotationX(float rad) {
        Mat4 r = identity(); float c = std::cos(rad), s = std::sin(rad);
        r.m[5] = c; r.m[6] = s; r.m[9] = -s; r.m[10] = c;
        return r;
    }
    static Mat4 rotationY(float rad) {
        Mat4 r = identity(); float c = std::cos(rad), s = std::sin(rad);
        r.m[0] = c; r.m[2] = -s; r.m[8] = s; r.m[10] = c;
        return r;
    }
    static Mat4 rotationZ(float rad) {
        Mat4 r = identity(); float c = std::cos(rad), s = std::sin(rad);
        r.m[0] = c; r.m[1] = s; r.m[4] = -s; r.m[5] = c;
        return r;
    }
    static Mat4 lookAt(const Vec3 &eye, const Vec3 &at, const Vec3 &up) {
        Vec3 f = (at - eye).normalized();
        Vec3 s = f.cross(up).normalized();
        Vec3 u = s.cross(f);
        Mat4 r = identity();
        r.m[0] = s.x; r.m[4] = s.y; r.m[8] = s.z;
        r.m[1] = u.x; r.m[5] = u.y; r.m[9] = u.z;
        r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
        r.m[12] = -s.dot(eye);
        r.m[13] = -u.dot(eye);
        r.m[14] = f.dot(eye);
        return r;
    }
    Mat4 operator*(const Mat4 &o) const {
        Mat4 r;
        for (int c = 0; c < 4; ++c)
            for (int row = 0; row < 4; ++row) {
                float v = 0;
                for (int k = 0; k < 4; ++k) v += m[k * 4 + row] * o.m[c * 4 + k];
                r.m[c * 4 + row] = v;
            }
        return r;
    }
    Vec3 transformPoint(const Vec3 &p) const {
        return {m[0] * p.x + m[4] * p.y + m[8] * p.z + m[12],
                m[1] * p.x + m[5] * p.y + m[9] * p.z + m[13],
                m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14]};
    }
    Vec3 transformVector(const Vec3 &p) const {
        return {m[0] * p.x + m[4] * p.y + m[8] * p.z,
                m[1] * p.x + m[5] * p.y + m[9] * p.z,
                m[2] * p.x + m[6] * p.y + m[10] * p.z};
    }
};

inline Mat4 operator*(const Mat4 &a, float s) {
    Mat4 r = a;
    for (float &v : r.m) v *= s;
    return r;
}

} // namespace rr4
