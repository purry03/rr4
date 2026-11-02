#!/usr/bin/env python3
"""Generate the ported collision switch from the original Lingo.

`doCollisions` in BehaviorScript 4 - PlayerCar.ls contains a ~2400-line
`case tempTileID of` block: for every original tile it clamps a tile-local
`pushOrigin` and calls `doRadialPush` once or twice. Those clamps depend on the
car's position inside the tile, the tile rotation and (for the airport tiles)
the plane collider, so they are not data that can be tabulated: they are code.

Rather than hand-transcribing the block (and risking a typo in one of the 549
`if`s), this script translates the decompiled Lingo into C++ mechanically and
writes `src/collision_cases.gen.cpp`. Run it whenever the dump is regenerated:

    python3 tools/gen_collisions.py [dump_dir] [out_file]
"""

import os
import re
import sys

DUMP = sys.argv[1] if len(sys.argv) > 1 else "work/dump/redlinerumble4"
OUT = sys.argv[2] if len(sys.argv) > 2 else "src/collision_cases.gen.cpp"
SRC = os.path.join(DUMP, "casts", "Internal", "BehaviorScript 4 - PlayerCar.ls")

PI = "3.14159265358979323846"
LABEL = re.compile(r"^\d+(, ?\d+)*:$")


class If:
    def __init__(self, cond, then, other):
        self.cond, self.then, self.other = cond, then, other


class Case:
    def __init__(self, expr, arms):
        self.expr, self.arms = expr, arms


class Assign:
    def __init__(self, lhs, rhs):
        self.lhs, self.rhs = lhs, rhs


class Call:
    def __init__(self, text):
        self.text = text


def load_case_block():
    lines = open(SRC, encoding="latin-1").read().split("\n")
    start = None
    for i, l in enumerate(lines):
        if l.strip() == "case tempTileID of":
            start = i
            break
    assert start is not None, "case tempTileID of not found"
    indent = len(lines[start]) - len(lines[start].lstrip())
    end = None
    for j in range(start + 1, len(lines)):
        l = lines[j]
        if l.strip() == "end case" and len(l) - len(l.lstrip()) == indent:
            end = j
            break
    assert end is not None, "matching end case not found"
    return [l for l in lines[start:end + 1] if l.strip()]


def parse_block(lines, i, stop, in_case=False):
    stmts = []
    while i < len(lines):
        t = lines[i].strip()
        if t in stop:
            return stmts, i
        if in_case and LABEL.match(t):
            return stmts, i
        m = re.match(r"^if (.*) then$", t)
        if m:
            then, i = parse_block(lines, i + 1, ("else", "end if"))
            other = []
            if i < len(lines) and lines[i].strip() == "else":
                other, i = parse_block(lines, i + 1, ("end if",))
            assert lines[i].strip() == "end if", lines[i]
            stmts.append(If(m.group(1), then, other))
            i += 1
            continue
        m = re.match(r"^case (.*) of$", t)
        if m:
            arms = []
            i += 1
            while lines[i].strip() != "end case":
                label = lines[i].strip()
                assert LABEL.match(label), label
                keys = [int(x) for x in label[:-1].split(",")]
                body, i = parse_block(lines, i + 1, ("end case",), in_case=True)
                arms.append((keys, body))
            i += 1
            stmts.append(Case(m.group(1), arms))
            continue
        m = re.match(r"^(.+?) = (.*)$", t)
        if m:
            stmts.append(Assign(m.group(1).strip(), m.group(2).strip()))
            i += 1
            continue
        stmts.append(Call(t))
        i += 1
    return stmts, i


def rename(stmts):
    for s in stmts:
        if isinstance(s, Assign):
            s.lhs = s.lhs.replace("g_DamagePercent", "damage")
            s.rhs = s.rhs.replace("g_DamagePercent", "damage")
        elif isinstance(s, If):
            s.cond = s.cond.replace("g_DamagePercent", "damage")
            rename(s.then)
            rename(s.other)
        elif isinstance(s, Case):
            s.expr = s.expr.replace("g_DamagePercent", "damage")
            for _, body in s.arms:
                rename(body)


def parse(block):
    stmts, i = parse_block(block, 0, ())
    rename(stmts)
    return stmts


# --- type inference -------------------------------------------------------

def is_vec_expr(rhs, vec_vars):
    if rhs.startswith("vector("):
        return True
    if re.search(r"\.(worldPosition|xAxis)\b", rhs):
        return True
    if ".getNormalized()" in rhs or ".duplicate()" in rhs:
        return True
    # A vector variable used for its components does not make the result a
    # vector: strip `name.x` style accesses first.
    stripped = rhs
    for name in vec_vars:
        stripped = re.sub(r"(?<![\w.])" + re.escape(name) +
                          r"\.(x|y|z)\b", "0", stripped)
    for name in vec_vars:
        if re.search(r"(?<![\w.])" + re.escape(name) + r"(?![\w.])", stripped):
            return True
    return False


def walk_assigned(stmts):
    for s in stmts:
        if isinstance(s, Assign):
            yield s
        elif isinstance(s, If):
            yield from walk_assigned(s.then)
            yield from walk_assigned(s.other)
        elif isinstance(s, Case):
            for _, body in s.arms:
                yield from walk_assigned(body)


def infer_types(stmts, known_vec):
    vec = set(known_vec)
    changed = True
    while changed:
        changed = False
        for a in walk_assigned(stmts):
            name = a.lhs
            if name in vec:
                continue
            if name.endswith((".x", ".y", ".z", ".worldPosition", ".xAxis")):
                base = name.rsplit(".", 1)[0]
                if base.endswith("]"):
                    base = base
                if base not in vec:
                    vec.add(base)
                    changed = True
                continue
            if is_vec_expr(a.rhs, vec):
                vec.add(name)
                changed = True
    return vec


# --- expression translation ----------------------------------------------

def tr_expr(e, vec):
    # Function/property forms first.
    e = re.sub(r"\bvector\(", "Vec3(", e)
    e = re.sub(r"\babs\(", "std::fabs(", e)
    e = re.sub(r"\bcos\(", "std::cos(", e)
    e = re.sub(r"\bsin\(", "std::sin(", e)
    e = re.sub(r"\btan\(", "std::tan(", e)
    e = re.sub(r"\bsqrt\(", "std::sqrt(", e)
    e = re.sub(r"([A-Za-z_][\w.\[\]]*)\.magnitude\b", r"\1.length()", e)
    e = re.sub(r"([A-Za-z_][\w.\[\]]*)\.getNormalized\(\)", r"\1.normalized()", e)
    e = re.sub(r"([A-Za-z_][\w.\[\]]*)\.duplicate\(\)", r"\1", e)
    e = e.replace("mVisTileList[tempTile].worldPosition", "tilePos")
    e = e.replace("gPlayerCar.worldPosition", "carPos")
    e = e.replace("gPlaneCollider.worldPosition", "ctx.planeWorld()")
    e = re.sub(r"gPlaneCollider\.transform\.position\.([xyz])", r"planeLocal.\1", e)
    e = e.replace("mRadar.transform.xAxis", "radarXAxis")
    e = re.sub(r"\bvelVector\b", "velVector", e)
    e = re.sub(r"\brotVector\b", "rotVector", e)
    e = e.replace("g_FPSFactor", "fps")
    e = e.replace("tempTileRot", "tileRot")
    e = e.replace("g_DamagePercent", "damage")
    e = re.sub(r"\bPI\b", PI, e)
    # `radians` is a property set in doSetupPlayerCar: radians = 180.0 / PI,
    # so `deg / radians` converts degrees to radians.
    e = re.sub(r"\bradians\b", "(180.0 / " + PI + ")", e)
    # Lingo operators.
    e = re.sub(r"\bmod\b", "%", e)
    e = re.sub(r"\band\b", "&&", e)
    e = re.sub(r"\bor\b", "||", e)
    e = re.sub(r"<>", "!=", e)
    # Lingo `=` is equality inside expressions.
    e = re.sub(r"(?<![<>=!])=(?!=)", "==", e)
    return e


def tr_stmt(s, vec, ind):
    pad = " " * ind
    if isinstance(s, If):
        out = [pad + "if (" + tr_expr(s.cond, vec) + ") {"]
        for x in s.then:
            out += tr_stmt(x, vec, ind + 2)
        if s.other:
            out.append(pad + "} else {")
            for x in s.other:
                out += tr_stmt(x, vec, ind + 2)
        out.append(pad + "}")
        return out
    if isinstance(s, Case):
        out = [pad + "switch (" + tr_expr(s.expr, vec) + ") {"]
        for keys, body in s.arms:
            for k in keys:
                out.append(pad + "case %d:" % k)
            for x in body:
                out += tr_stmt(x, vec, ind + 2)
            out.append(pad + "  break;")
        out.append(pad + "}")
        return out
    if isinstance(s, Assign):
        lhs = s.lhs
        if lhs == "gPlaneCollider.worldPosition":
            return [pad + "planeLocal = ctx.planeToLocal(" + tr_expr(s.rhs, vec) + ");"]
        m = re.match(r"gPlaneCollider\.transform\.position\.([xyz])$", lhs)
        if m:
            return [pad + "planeLocal." + m.group(1) + " = " + tr_expr(s.rhs, vec) + ";"]
        if lhs.endswith(".normalize()"):
            base = lhs[:-len(".normalize()")]
            return [pad + base + " = " + base + ".normalized();"]
        return [pad + lhs + " = " + tr_expr(s.rhs, vec) + ";"]
    t = s.text
    m = re.match(r"^doRadialPush\([^,]+, [^,]+, ([^,]+), ([^,]+), (\d+), (\d+)\)$", t)
    if m:
        return [pad + "ctx.push(" + tr_expr(m.group(1), vec) + ", " +
                tr_expr(m.group(2), vec) + ", " + m.group(3) + ", " + m.group(4) + ");"]
    m = re.match(r"^(\w+)\.normalize\(\)$", t)
    if m:
        return [pad + m.group(1) + " = " + m.group(1) + ".normalized();"]
    if re.match(r"^puppetSound\(", t):
        return [pad + "playHit(" + re.search(r"member\((\d+)", t).group(1) + ");"]
    if re.match(r"^soundMemberList\[", t):
        return [pad + "// " + t]
    raise SystemExit("unhandled statement: " + t)


def main():
    block = load_case_block()
    root = parse(block)

    known = {"pushOrigin", "tilePos", "carPos", "velVector", "rotVector",
             "planeColliderPos", "radarXAxis"}
    vec = infer_types(root, known)

    names = set()
    for a in walk_assigned(root):
        base = a.lhs.split(".")[0]
        if base in ("pushOrigin", "gPlayerCar", "mVisTileList[tempTile]",
                    "mRadar", "gPlaneCollider", "velVector", "rotVector"):
            continue
        names.add(base)
    bound = {"damage", "tempRadius"}
    scalars = sorted(n for n in names if n not in vec and "[" not in n and n not in bound)
    vectors = sorted(n for n in names if n in vec and "[" not in n)

    cases = []
    for k, (keys, body) in enumerate(root[0].arms if isinstance(root[0], Case) else []):
        pass

    # The block is a single `case tempTileID of`.
    assert len(root) == 1 and isinstance(root[0], Case), [type(x).__name__ for x in root]
    tile_case = root[0]
    print("arms:", len(tile_case.arms))
    print("scalars:", scalars)
    print("vectors:", vectors)

    out = []
    out.append("// Generated by tools/gen_collisions.py from")
    out.append("// BehaviorScript 4 - PlayerCar.ls, doCollisions() case tempTileID of.")
    out.append("// Do not edit by hand: rerun the generator after regenerating the dump.")
    out.append("")
    out.append('#include "collision.hpp"')
    out.append("")
    out.append("#include <cmath>")
    out.append("")
    out.append("namespace rr4 {")
    out.append("")
    out.append("namespace {")
    out.append("using Vec3 = rr4::Vec3;")
    out.append("constexpr double PI = %s;" % PI)
    out.append("} // namespace")
    out.append("")
    out.append("void collideTile(CollisionCtx &ctx, int tileID, int tileRot, int onTile) {")
    out.append("    Vec3 &pushOrigin = ctx.origin;")
    out.append("    float &tempRadius = ctx.radius;")
    out.append("    Vec3 &tilePos = ctx.tilePos;")
    out.append("    Vec3 &carPos = ctx.carPos;")
    out.append("    Vec3 &velVector = ctx.vel;")
    out.append("    Vec3 &rotVector = ctx.rot;")
    out.append("    Vec3 &planeLocal = ctx.planeLocal;")
    out.append("    Vec3 radarXAxis = ctx.radarXAxis;")
    out.append("    float fps = ctx.fps;")
    out.append("    float &damage = ctx.damage;")
    out.append("    (void)onTile; (void)pushOrigin; (void)tempRadius; (void)tilePos;")
    out.append("    (void)carPos; (void)velVector; (void)rotVector; (void)planeLocal;")
    out.append("    (void)radarXAxis; (void)fps; (void)damage;")
    for n in scalars:
        out.append("    float %s = 0.0f;" % n)
    for n in vectors:
        out.append("    Vec3 %s{0, 0, 0};" % n)
    out.append("")
    out.append("    switch (tileID) {")
    for keys, body in tile_case.arms:
        for k in keys:
            out.append("    case %d:" % k)
        for s in body:
            out += tr_stmt(s, vec, 8)
        out.append("        break;")
    out.append("    default:")
    out.append("        break;")
    out.append("    }")
    out.append("}")
    out.append("")
    out.append("} // namespace rr4")
    out.append("")

    os.makedirs(os.path.dirname(OUT) or ".", exist_ok=True)
    open(OUT, "w").write("\n".join(out))
    print("wrote", OUT, len(out), "lines")


if __name__ == "__main__":
    main()
