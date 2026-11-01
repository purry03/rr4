# Redline Rumble 4 - native C++ port

A from-scratch C++20/SFML reimplementation of the playable car of
**Redline Rumble 4** (Adobe Director 11.5.9, `redlinerumble4.dcr`), built by
decompiling the original Lingo and using the original Shockwave 3D geometry and
textures.

Scope of this build: **all six original player cars, the whole 12 x 12 map,
free driving.** The default is car #1 (MeanieCropper), the car owned by the
original save (`g_CarsOwnedList = [2,0,0,0,0,0]`); `--car N` switches to any of
the other five.
The original music and car sounds are played through the same Director sound
channels. Traffic, opponents, pickups and the race/stunt logic are intentionally
not included.

## Build

Dependencies: CMake >= 3.16, a C++20 compiler, SFML 3.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
```

## Run

```bash
./build/rr4              # car #1, MeanieCropper
./build/rr4 --car 4      # any of the six original cars
```

The car starts where the original `doSetupPlayerCar` puts it:
`worldPosition = (32053.57, -5.56, 30797.51)`, `rotation.y = 45`, which is the
Ferris wheel tile (`TILE_FerrisWheel_000`, the "FERRIS' WHEELS / UNITED AUTO
SALES" frontage).

Controls:

| Key | Action |
| --- | --- |
| Up / Down | accelerate / brake + reverse |
| Left / Right | steer |
| Shift | nitro boost |
| M | mute / unmute (original MUTE+PAUSE button) |
| F12 | save a PNG screenshot to `work/shots/` |
| F1 | toggle the optional debug readout (speed, tile, tilt, FPS) |
| Esc | quit |

The in-game HUD is the original one: score digits (top left), damage bar and
skull, boost bar with the `igNitro` logo (top right), race timer, the radar
ring (bottom left) and the mute speaker button (bottom right). It is drawn from
the original Director camera-overlay bitmaps at their original stage
coordinates; see "Original HUD" below.

Set `RR4_NEAREST=1` to disable bilinear texture filtering if you prefer the
faster nearest-neighbour path.

## Original assets

The game no longer uses procedural geometry. Everything visible comes from the
3D cast member stored in the original Director file:

- `work/dump2/redlinerumble4/chunks/XMED-6650.bin` is the original Shockwave 3D
  scene (2.5 MB, `3DEM` stream, 163 meshes, 51,591 faces, 133,112 vertices).
- The original web build's Shockwave 3D decoder was run in Node to unpack the
  stream. Its output is packed into `assets/rr4_world.bin` (5.03 MB, magic
  `RR4W`, version 1): all meshes, material groups, original JPEG/PNG texture
  bytes, UVs and normals.
- `assets/textures/` keeps the decoded original texture files for inspection.
- `src/assets.cpp` loads the pack at startup and decodes the textures with
  SFML. `src/world.cpp` assembles the map, `src/car.cpp` draws the player car.
- The 3DOM UVs use a bottom-left origin (OpenGL convention) while the packed
  textures are stored top-down. `src/assets.cpp` flips V while loading, so
  building signs/windows and the wheel sidewall/rim textures are upright.
- `assets/hud/` holds the original HUD bitmap cast members (PNG with their
  original alpha), extracted from the Director `BITD`/`ediM` + `ALFA` chunks by
  `tools/extract_hud.py`.

Car assembly and physics come from `doSetupShells` / `doControls` in the
decompiled `BehaviorScript 4 - PlayerCar.ls`. All six cars are data-driven by
the `kCars` table in `src/car.cpp` (`CarConfig`): mesh names, bonnet/boot/door
and tire pivots, rear-axle offset, acceleration, paint tint, shell material and
engine Sfx members. The original `BODY`, `BONNET`, `BOOT`, `DOOR` and `TIRE`
meshes are the +X half of each car and are mirrored with `scale(-1, 1, 1)`.
The rear axle is the front wheel clone translated by the car's original
`tempTireBL.translate` value (MeanieCropper: `(0, 0, -130)`), exactly as the
script does. Acceleration is the per-car value from `doControls` (car #1 =
`0.025` through car #6 = `0.0425`). The original game swaps a Director
cast-member texture for the car paint; those cast members are not part of the
3DOM stream, so each original shell texture is tinted with its livery colour.

## Original HUD

The Director game draws its HUD as 2D camera overlays on top of the 3D view,
not as part of the scene. The port reproduces them one-for-one on an 800 x 480
logical stage:

| Overlay | Member | Position | Source |
| --- | --- | --- | --- |
| score (5 green digits) | 25..34 `SmallGreenNumbers` | (4 + 26*i, 28) | `Score&Timer.ls` |
| timer digits | 37..46 `MagentaNumbers` | 640 / 692 / 718, then 756 / 772 at scale 0.5 | `Score&Timer.ls` |
| timer colon | 47 | (664, 28) | `Score&Timer.ls` |
| damage bar / red fill | 12 / 13 | (0, 8), fill at `-254 + 254 * damage` | `Overlays.ls` |
| damage skull | 14 | (214, 0) | `Overlays.ls` |
| boost bar / yellow fill | 16 / 17 | (544, 8), fill at `800 - 250 * boost` | `Overlays.ls` |
| boost `igNitro` logo | 18 | (505, 2) | `Overlays.ls` |
| radar ring | 54 | (20, 400) | `doSetupRadar()` |
| mute speaker | 1 / 2 | (776, 460) | `MUTE+PAUSE_Button.ls` |

`tools/extract_hud.py` parses the cast slot table, the `KEY` resource table and
the original `CASt` metadata, then decodes JPEG (`ediM`) and Director RLE
(`BITD`) images with their 8-bit `ALFA` masks. 37 PNGs are written to
`assets/hud/`. `src/hud.cpp` loads them and uses the exact overlay positions,
scales and alpha blending from the Lingo. The old custom text/minimap HUD is
gone; F1 shows a separate debug readout for development.

The message panel (`InfoPanels.ls`) and the pause screen
(`MUTE+PAUSE_Button.ls`) use the same overlay mechanism and can be added on top
of this HUD later.

## Original sound

`tools/extract_sounds.py` reads the `Sfx` cast slot table (`CAS_-6473.json`),
the `KEY` sound-section table and the `snd` chunks, and writes the original
uncompressed 16-bit mono 44.1 kHz PCM as `assets/sounds/<cast name>.wav`
(32 members, 19 MB). `assets/sounds/index.txt` keeps the slot/cast/chunk map.

`src/audio.*` reproduces the Director sound channels used by the car:

| Channel | Original member | Use |
| --- | --- | --- |
| 1 | `CrashTinkle_*` 16..19, `CrashLanding` 20, `SuspensionCreak` 21 | impacts, hard landings, creaks |
| 2 | `<car>_RevvingUp/Down`, members `playerCarID*2-1` / `*2` | engine, selected by `Car::setCar()` |
| 2 | `Bitsasushi_Braking` 14 / `Reversing` 13 | overrides the engine while Down is held |
| 3 | `igNitroIgnition` 23 | boost ignition |
| 4 | `Bitsasushi_SkiddingLeft/Right` 42/43 | tire skid |
| 5 | `ChemicalBro_loop_001` 15 | music, looped from `doSetupPlayerCar` |

`doEngineSounds()` is ported as the original two-state machine: rev-up while
Up is held, rev-down after release, with `soundCompletePercent` used to resume
the next loop at the matching offset. All six engine pairs load at startup, so
switching cars changes the engine sound together with the shell. The Mute button pauses channel 5 and
stops channels 1..4, exactly like `MUTE+PAUSE_Button.ls`.

## Map and rendering

- All 144 tiles from the original `tileIDList`, each rotated by
  `tileRotList` (0..3 -> 0/90/180/270 degrees). The map uses the original
  1-based `onTile = onTileCol + (onTileRow - 1) * 12` indexing and tile centres
  at `col * 5000 - 2500`, so the tiles, the tall objects and the car spawn all
  share one coordinate frame.
- The 37 original tile resources are drawn from their base and objects meshes,
  grouped by material. The 5 x 5 visible-tile window matches the original
  `visTileDist = 2`.
- Tall objects are placed with the exact positions from `doSetupTallObjects`:
  Sunny Pierce Toole, the Golden Gate Bridge (plus the rotated/translated
  clone), the Wellard screwdriver, TahTee Cosmetics, the Lunkopf & Stubbs
  cigarette, the airport radar and the parked Boeing 747.
- The original `Landscape_TempSkyBox_000` follows the camera.
- Baked tile shadow planes are skipped because the original sets
  `shadowsOn = 0` on a software renderer. The car uses a soft blob shadow.
- A 128 x 128 height field per tile is built from the original base meshes.
  The car is tilted by sampling ground under the four wheels, the height-field
  equivalent of the original's four lowest cage rays (`x = +-50`, `z = +-60`):
  pitch follows the front/rear height difference, roll follows the left/right
  difference, and the shell visibly leans into the banked NurdBurger corners,
  bridge ramps and canal slopes. Support height comes from under the car
  centre, with the same ~90-unit ray reach as the original, so overhead decks
  and canal beds do not twist the body.
- Tile batches are drawn for the whole 5 x 5 window and left to the rasterizer's
  near-plane clipper; there is no per-tile "behind the camera" rejection, so
  road immediately behind the car is never dropped.
- The chase camera is a direct port of `doCam()`: the initial `camAim` and
  `cam.worldPosition` offsets, `lookingForwards` swing when throttle > 6,
  `camRange` 320/480, `camHeight` 120/200, the 10 % aim and height easing and
  the tile-7/tile-20 camera state. There is no extra obstruction raycast; the
  original camera does not use one.
- Software rasterizer in `src/renderer.cpp`: perspective-correct textures,
  bilinear filtering near the camera, z-buffer, near-plane clipping,
  back-face culling (winding normalised against the authored normals),
  directional shading and distance fog. Framebuffer is uploaded to an SFML
  texture each frame.

## Original collisions

`doCollisions()` does not use boxes or swept shapes. For each of the four tiles
around the car (the car's own tile plus the three sharing the nearest tile
corner) it builds a tile-local `pushOrigin` and calls `doRadialPush()`: the car
is pushed out of a circle of a per-tile radius centred on that point. The
clamps that build `pushOrigin` are hand-tuned per tile and depend on the car's
position inside the tile, the tile's rotation (`tileRotList`), the airport
radar's spinning `xAxis` and the plane collider, so they are code rather than
data.

`tools/gen_collisions.py` translates the 2400-line `case tempTileID of` block
from the decompiled `BehaviorScript 4 - PlayerCar.ls` into
`src/collision_cases.gen.cpp` mechanically, which keeps all 549 `if`s and 568
coordinate clamps byte-for-byte faithful. `src/collision.cpp` implements
`doRadialPush()`:

```
diff = tilePos + pushOrigin - (carPos + vel * g_FPSFactor)
if |diff| < radius:
    push = normalize(diff) * (radius - |diff|)
    vel -= push / g_FPSFactor
    rot.z -= push.x * 0.4 / g_FPSFactor
    rot.x += push.z * 0.4 / g_FPSFactor
```

with the original damage/Sfx thresholds at `|push| > 5 / 10 / 15 / 20`
(0.005 / 0.01 / 0.015 / 0.02 damage and Sfx 16..19). Damage feeds the original
HUD damage bar and the crash sound uses the existing CrashTinkle channel.

The pass runs at the same point in the frame as the original:
`doControls → doGravity → doFriction → doTireGrip → doCollisions → doBodyRoll →
doupdate`. `Car::update()` takes a collision hook and calls it between
`doTireGrip` and `doBodyRoll`, so the pushes act on the current velocity while
the car is still at its pre-move position; `doupdate` then carries the pushed
velocity out of the obstacle. `World::updateGround()` (the `doRayCasting` ground
follow and the map-edge fence) runs after the step. Running collisions after the
move instead makes the pushed velocity re-trigger on the next frame, which shows
up as the car bouncing off walls — that ordering is the difference.

The tile grid uses the original indexing: `onTileCol = integer(x / 5000 + 0.5)`,
tile centres at `col * 5000 - 2500` (the `mVisTileList[]` world position) and
the flat index `(col - 1) + (row - 1) * 12`. This is why the spawn is on the
Ferris wheel tile and every tall object sits on its original tile.

## Verification modes

```bash
# Drive with a waypoint autopilot and save screenshots every 300 frames.
./build/rr4 --auto 4800 --shots work/final_shots
./build/rr4 --car 4 --auto 4800 --shots work/final_shots_highwire
./build/rr4 --auto 4800 --shots work/final_shots --shot-every 60

# Report the settled ground height and surface pitch/roll at one spot.
./build/rr4 --probe 30000 47000 0

# List the steepest original road surfaces (banked corners, ramps).
./build/rr4 --slope-scan

# Hold the throttle from a placed position to observe collision pushes.
./build/rr4 --drive 240 --place 22500 4500 0

# Measure the software rasterizer.
./build/rr4 --bench 360

# Dump 300 frames of raw car state, then compare against an independent
# Python reimplementation of the decompiled Lingo step.
./build/rr4 --trace 300 > work/trace.csv
python3 tools/verify_physics.py work/trace.csv

# Dump the camera state and compare against an independent doCam() model.
./build/rr4 --cam-trace 600 > work/cam_trace.csv
python3 tools/verify_camera.py work/cam_trace.csv

# Load and exercise the original Sfx cast without a window.
./build/rr4 --audio-test
```

Results from this checkout:

- Auto drive (car #1): the route covers the docks, Golden Gate bridge,
  Financial district, Cisco Hill, NurdBurger ring and Ferris wheel; screenshots
  are written to `work/final_shots/`.
- Software renderer: 640 x 400 framebuffer (800 x 480 HUD stage) upscaled to a
  960 x 600 window, ~25 ms/frame, ~40 fps (`--bench 120` on this machine).
- Physics check: every field of the ported step matches the independent model
  within float tolerance over 300 frames; worst position error 0.011 units
  (`RESULT: PASS`).
- Camera check: `camRange`, `camHeight`, `camAim` and `cam.worldPosition`
  match the independent doCam model over 600 frames; worst axis error
  0.005 units (`RESULT: PASS`).
- Audio check: all 32 original Sfx members load and the engine, brake, nitro
  and mute path runs (`--audio-test`).

## What was ported

All car values come from
`work/dump/redlinerumble4/casts/Internal/BehaviorScript 4 - PlayerCar.ls`
(ProjectorRays output). The default trace follows the car #1 (MeanieCropper)
branch; the other five are in the same `kCars` table:

| Original | Port |
| --- | --- |
| start position `(32053.57, -5.56, 30797.51)` | `Car::pos`, ground plane at Y = 0 |
| start rotation `(0, 45, 0)` | `Car::yaw = 45 deg` |
| steering +/-2/frame, return 5/frame, clamp +/-40 | `Car::update` |
| throttle +/-0.2/frame, clamp -8..10 | `Car::update` |
| accel `vel += forward * throttle * 0.025` (car 1) .. `0.0425` (car 6) | `Car::update` |
| nitro: second 0.025 accel, 0.005/frame drain | `Car::update` |
| gravity 1/frame, Y damping 5 %, drag 0.5 %/frame | `Car::update` |
| yaw damping 16 %/frame, `steerVel = clamp(steer * 0.001 * forwardSpeed, +/-0.5)` | `Car::update` |
| tyre grip: strip lateral velocity, capped at 1/frame | `Car::update` |
| body roll `rotVector.x/z` from `prevVel - vel` | `Car::rotXDeg/rotZDeg` |
| cage-ray ground response (`toCenter`, `imbedCap`) | `World::resolveCarCollision` surface pitch/roll |
| camera FOV 55, range 320/480, height 120/200, aim +90, `lookingForwards` | `ChaseCam::update` (`doCam`) |
| `tileIDList` / `tileRotList` (144 cells) | `src/world.cpp`, byte-for-byte values |
| 37 tile resource names | `kTileNames` |
| `col = int(x/5000 + 0.5)`, `row = int(z/5000 + 0.5)` | `World::tileColAt/tileRowAt` |

The per-frame update order follows the original `enterFrame`:

```
doEngineSounds -> doControls -> doGravity -> doFriction -> doTireGrip
  -> doBodyRoll -> doupdate -> doAnims -> doCam
```

Fixed 60 Hz stepping with an accumulator; `g_FPSFactor` is 1.0 per step.

## Repository layout

```
src/math.hpp        vectors, matrices
src/assets.*        RR4W pack loader + original mesh/material drawing
src/renderer.*      software rasterizer
src/world.*         original tile map, props, height field
src/collision.*     doRadialPush() and the four-tile collision pass
src/collision_cases.gen.cpp  per-tile clamps generated from the Lingo
src/car.*           ported car physics and all six original car assemblies
src/audio.*         original Sfx cast and Director sound channels
src/hud.*           original camera-overlay HUD from assets/hud/
src/font.*          5 x 7 bitmap font used by the F1 debug readout
src/main.cpp        window, fixed-step loop, camera, HUD, headless modes
assets/rr4_world.bin        packed original geometry/materials/textures
assets/textures/            decoded original textures
assets/sounds/              original Sfx cast members as WAV
assets/hud/                 original HUD bitmap cast members as RGBA PNG
tools/extract_sounds.py     extract Sfx cast from the Director dump
tools/extract_hud.py        extract and decode the HUD bitmaps (BITD/ediM/ALFA)
tools/gen_collisions.py     translate doCollisions() into collision_cases.gen.cpp
tools/verify_physics.py     independent check of the ported car step
tools/verify_camera.py      independent check of doCam()
work/final_shots/           auto-drive screenshots (evidence)
```

## Decompilation source

```bash
projectorrays decompile redlinerumble4.dcr -o work/dump --dump-chunks
```

The decompiled Lingo and all cast chunks live under `work/dump/` (scripts,
`XMED` W3D models, `snd`, `BITD`, `ediM` JPEGs). The original car behaviour is
`BehaviorScript 4 - PlayerCar.ls`; the timing behaviour is
`BehaviorScript 5 - FPS.ls`.

## Known gaps

- The Shockwave 3D stream does not expose the node transforms for the car
  parts (only five node records parse), so bonnet, boot, door and wheel pivots
  were reconstructed from the cage and the part geometry. They are very close
  but not byte-identical to the Director hierarchy.
- Traffic cars, the opponent car and the moving plane are not implemented, so
  their `doCollisions` entries (traffic push, plane collider) are inactive.
  Every static tile collider is ported exactly.
- Canal water is treated as drivable ground in free-drive mode, so the
  autopilot can cut a corner and end up in the canal.
- Surface tilt is a four-wheel height-field approximation of the original
  twelve-ray cage. A wheel hanging over a kerb or canal edge can push the tilt
  to its 30-degree clamp; the original ray test also reacts strongly there.
- The 12-ray cage ground test is approximated by a 128 x 128 per-tile height
  field, so steep tree-covered hillsides can still put the view close to
  foliage.
- Audio covers the car channels (music, engine, brake/reverse, skid,
  suspension/landing, nitro and impacts). Pickup, race-pod, Ferris-wheel,
  repair-garage and menu sounds are extracted but not triggered because those
  systems are outside this free-drive build.
- No traffic, opponents, race flags, points or damage.
