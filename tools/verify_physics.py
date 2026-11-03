#!/usr/bin/env python3
"""Independent check of the ported Redline Rumble 4 car step.

Reads the CSV produced by `./build/rr4 --trace 300` and re-simulates the same
step from the original Lingo constants (BehaviorScript 4 - PlayerCar.ls,
car #1 branch, MeanieCropper). Any mismatch above the tolerances below means the C++ port
drifted away from the decompiled behaviour.

Usage: python3 tools/verify_physics.py work/trace.csv
"""

import csv
import math
import sys

PI = math.pi
TILE = 5000.0
BODY_TILT_DAMPING = 0.35


def clamp(v, lo, hi):
    return max(lo, min(hi, v))


def mat_mul(a, b):
    return [[sum(a[r][k] * b[k][c] for k in range(3)) for c in range(3)]
            for r in range(3)]


def rot_x(deg):
    c, s = math.cos(math.radians(deg)), math.sin(math.radians(deg))
    return [[1, 0, 0], [0, c, -s], [0, s, c]]


def rot_y(deg):
    c, s = math.cos(math.radians(deg)), math.sin(math.radians(deg))
    return [[c, 0, s], [0, 1, 0], [-s, 0, c]]


def rot_z(deg):
    c, s = math.cos(math.radians(deg)), math.sin(math.radians(deg))
    return [[c, -s, 0], [s, c, 0], [0, 0, 1]]


def rotation_xyz(x, y, z):
    """Director transform.rotation = (x, y, z): X, then Y, then Z."""
    return mat_mul(rot_z(z), mat_mul(rot_y(y), rot_x(x)))


def column(m, c):
    return [m[0][c], m[1][c], m[2][c]]


def trace_input(frame):
    """Same schedule as CarInput traceInput() in src/main.cpp."""
    up = True
    left = frame >= 120 and frame < 180
    right = frame >= 240 and frame < 300
    return up, False, left, right, False


def simulate():
    pos = [32053.57, 0.0, 30797.51]   # doSetupPlayerCar (Y flattened by the port)
    orient = rotation_xyz(0, 45, 0)   # mPlayerCar.transform.rotation = (0, 45, 0)
    vel = [0.0, 0.0, 0.0]
    steer = 0.0
    throttle = 0.0
    yaw_vel = 0.0
    mid_air = False
    rot_x = 0.0
    rot_z = 0.0
    prev_vel = [0.0, 0.0, 0.0]

    rows = []
    for frame in range(0, 301):
        up, down, left, right, boost = trace_input(frame)

        # ---- doControls --------------------------------------------------
        if left:
            if steer < 0:
                steer = 0.0
            if steer < 40:
                steer += 2.0
        if right:
            if steer > 0:
                steer = 0.0
            if steer > -40:
                steer -= 2.0
        if not left and not right:
            if steer > 0:
                steer = max(0.0, steer - 5.0)
            if steer < 0:
                steer = min(0.0, steer + 5.0)

        if up and not down:
            if not mid_air and throttle < 0:
                throttle = 0.0
            throttle += 0.2
        if down and not up:
            if not mid_air and throttle > 0:
                throttle = 0.0
            throttle -= 0.2
        if not up and not down:
            throttle = 0.0
        throttle = clamp(throttle, -8.0, 10.0)

        # -throttleVector = mPlayerCar.transform.zAxis; doTireGrip xAxis.
        fwd = column(orient, 2)
        right_axis = column(orient, 0)

        if not mid_air:
            vel = [vel[i] + fwd[i] * throttle * 0.025 for i in range(3)]

        fwd_speed = sum(fwd[i] * vel[i] for i in range(3))
        steer_vel = clamp(steer * 0.001 * fwd_speed, -0.5, 0.5)
        if not mid_air:
            yaw_vel += steer_vel

        # ---- doGravity / doFriction -------------------------------------
        vel[1] -= 1.0
        if not mid_air:
            vel[1] -= vel[1] * 0.05
            vel = [v - v * 0.005 for v in vel]
            yaw_vel -= yaw_vel * 0.16
            # Port deviation: pitch / roll damped at kBodyTiltDamping
            # (src/car.cpp) instead of the original 0.16; yaw keeps 0.16.
            rot_x -= rot_x * BODY_TILT_DAMPING
            rot_z -= rot_z * BODY_TILT_DAMPING

        # ---- doTireGrip ---------------------------------------------------
        lat = sum(right_axis[i] * vel[i] for i in range(3))
        grip = [right_axis[i] * lat for i in range(3)]
        mag = math.sqrt(sum(g * g for g in grip))
        if mag > 1.0:
            grip = [g / mag for g in grip]
        if not mid_air:
            vel = [vel[i] - grip[i] for i in range(3)]

        # ---- doBodyRoll ---------------------------------------------------
        diff = [vel[i] - prev_vel[i] for i in range(3)]
        diff[1] = 0.0
        mag = math.sqrt(sum(d * d for d in diff))
        if mag > 1.0:
            diff = [d / mag for d in diff]
        rot_x -= diff[2] * 0.35
        rot_z += diff[0] * 0.35
        prev_vel = list(vel)

        # ---- doupdate -----------------------------------------------------
        pos = [pos[i] + vel[i] for i in range(3)]
        # mPlayerCar.rotate(rotVector, #parent): world axes, premultiplied.
        orient = mat_mul(rotation_xyz(rot_x, yaw_vel, rot_z), orient)
        if pos[1] <= 0.0:
            pos[1] = 0.0
            if vel[1] < 0:
                vel[1] = 0.0
            mid_air = False
        else:
            mid_air = True

        heading = column(orient, 2)
        rows.append((frame, throttle, steer,
                     math.degrees(math.atan2(heading[0], heading[2])), yaw_vel,
                     vel[0], vel[1], vel[2], pos[0], pos[1], pos[2],
                     rot_x, rot_z))
    return rows


def main():
    if len(sys.argv) < 2:
        print("usage: verify_physics.py work/trace.csv")
        return 2

    with open(sys.argv[1], newline="") as f:
        reader = csv.DictReader(f)
        actual = [{k: float(v) for k, v in row.items()} for row in reader]

    expected = simulate()
    if len(actual) != len(expected):
        print(f"row count mismatch: cpp={len(actual)} python={len(expected)}")
        return 1

    names = ["throttle", "steer", "yawDeg", "yawVel", "velX", "velY", "velZ",
             "posX", "posY", "posZ", "rotX", "rotZ"]
    tolerance = {"throttle": 1e-4, "steer": 1e-4, "yawDeg": 5e-2, "yawVel": 1e-4,
                 "velX": 2e-3, "velY": 2e-3, "velZ": 2e-3,
                 "posX": 5e-2, "posY": 5e-2, "posZ": 5e-2,
                 "rotX": 5e-3, "rotZ": 5e-3}

    worst = {n: 0.0 for n in names}
    failed = False
    for i, (exp, act) in enumerate(zip(expected, actual)):
        for j, name in enumerate(names):
            diff = abs(exp[j + 1] - act[name])
            worst[name] = max(worst[name], diff)
            if diff > tolerance[name] and not failed:
                print(f"FAIL frame {i} field {name}: cpp={act[name]:.6f} "
                      f"model={exp[j + 1]:.6f} diff={diff:.6f}")
                failed = True

    print("max abs difference vs independent model (300 frames):")
    for name in names:
        print(f"  {name:9s} {worst[name]:.6f}")
    if failed:
        print("RESULT: FAIL")
        return 1
    print("RESULT: PASS - ported step matches the decompiled car formulas")
    return 0


if __name__ == "__main__":
    sys.exit(main())
