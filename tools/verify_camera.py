#!/usr/bin/env python3
"""Independent check of the ported doCam() camera.

Reads `./build/rr4 --cam-trace 600` and re-runs the original doCam equations
(BehaviorScript 4 - PlayerCar.ls) over the recorded car state. Checks camRange,
camHeight, camAim and cam.worldPosition every frame.

Usage: python3 tools/verify_camera.py work/cam_trace.csv
"""

import csv
import math
import sys

PI = math.pi


def norm(v):
    m = math.sqrt(sum(c * c for c in v))
    return [c / m for c in v] if m > 1e-9 else [0.0, 0.0, 0.0]


def simulate(rows):
    # doSetupPlayerCar worldPosition (the car settles to ground on frame 1).
    p0 = (32053.57, -5.56, 30797.51)
    # doSetupPlayerCar initial camera and then one doCam per enterFrame.
    aim = [p0[0] - 80.0, p0[1] + 50.0, p0[2] + 200.0]
    cam = [p0[0] - 80.0, p0[1] + 50.0, p0[2] - 200.0]
    cam_range, cam_height = 200.0, 50.0
    looking_forwards = 0
    out = []

    for row in rows:
        throttle = float(row["throttle"])
        pos = [float(row["posX"]), float(row["posY"]), float(row["posZ"])]
        vel = [float(row["velX"]), float(row["velY"]), float(row["velZ"])]
        tile = int(row["tile"])

        if throttle > 6.0:
            looking_forwards = 1
        if throttle < -7.0:
            looking_forwards = -1
        if throttle < 0.0:
            looking_forwards = 0

        # mPlayerCar.getWorldTransform().zAxis / .xAxis (pitch and roll
        # included); doCam flattens them itself.
        fwd = [float(row["zAxisX"]), float(row["zAxisY"]), float(row["zAxisZ"])]
        right = [float(row["xAxisX"]), float(row["xAxisY"]), float(row["xAxisZ"])]
        to_cam = [cam[0] - pos[0], 0.0, cam[2] - pos[2]]
        to_cam = norm(to_cam)
        side = norm([-right[0], 0.0, -right[2]])
        front = norm([-fwd[0], 0.0, -fwd[2]])
        front_sign = sum(front[i] * to_cam[i] for i in range(3))
        temp_rot = sum(side[i] * to_cam[i] for i in range(3))
        if looking_forwards > 0 and front_sign < 0:
            temp_rot = 1.0 if temp_rot > 0 else -1.0
        temp_rot = -temp_rot if looking_forwards > 0 else 0.0

        offset = norm([cam[0] - pos[0], 0.0, cam[2] - pos[2]])
        if temp_rot != 0.0:
            a = math.radians(temp_rot)
            c, s = math.cos(a), math.sin(a)
            offset = [c * offset[0] + s * offset[2], 0.0,
                      -s * offset[0] + c * offset[2]]

        state = 2 if tile in (7, 20) else 1
        target_range = 480.0 if state == 2 else 320.0
        target_height = 200.0 if state == 2 else 120.0
        cam_range += (target_range - cam_range) * 0.05
        cam_height += (target_height - cam_height) * 0.05

        target = [pos[i] + vel[i] + (90.0 if i == 1 else 0.0) for i in range(3)]
        aim = [aim[i] + (target[i] - aim[i]) * 0.10 for i in range(3)]
        aim[0] = pos[0]
        aim[2] = pos[2]

        cam[1] += (pos[1] + cam_height - cam[1]) * 0.10
        cam[0] = pos[0] + offset[0] * cam_range
        cam[2] = pos[2] + offset[2] * cam_range

        out.append((cam_range, cam_height, aim[0], aim[1], aim[2],
                    cam[0], cam[1], cam[2]))

    return out


def main():
    if len(sys.argv) < 2:
        print("usage: verify_camera.py work/cam_trace.csv")
        return 2
    with open(sys.argv[1], newline="") as f:
        rows = list(csv.DictReader(f))

    expected = simulate(rows)
    names = ["range", "height", "aimX", "aimY", "aimZ", "camX", "camY", "camZ"]
    keys = ["range", "height", "aimX", "aimY", "aimZ", "camX", "camY", "camZ"]
    worst = {n: 0.0 for n in names}
    failed = False
    for i, (row, exp) in enumerate(zip(rows, expected)):
        for j, name in enumerate(names):
            diff = abs(exp[j] - float(row[keys[j]]))
            worst[name] = max(worst[name], diff)
            # Camera positions reach ~60000 units and the game accumulates them
            # in float32, so the tolerance is absolute for the small range and
            # height channels and relative for the world-space ones.
            tol = 1e-3 + 1e-6 * abs(exp[j])
            if diff > tol and not failed:
                print(f"FAIL frame {i} {name}: cpp={row[keys[j]]} model={exp[j]:.6f}")
                failed = True

    print(f"max abs difference vs independent doCam model ({len(rows)} frames):")
    for n in names:
        print(f"  {n:9s} {worst[n]:.6f}")
    if failed:
        print("RESULT: FAIL")
        return 1
    print("RESULT: PASS - ported camera matches doCam()")
    return 0


if __name__ == "__main__":
    sys.exit(main())
