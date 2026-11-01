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
nearest-neighbour look.

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
`0.025` through car #6 = `0.0425`). Paint is the original runtime texture
swap: `shaderList[n].texture.member = member(150 + colourID, "Textures")`, with
the 2 x 2 paint bitmaps (members 151..158, `assets/hud/PaintN.png`) and the
default colour IDs `[1, 2, 3, 4, 5, 1]` (red, green, blue, green/yellow, black,
red).

Part pivots are the BONNET / BOOT / DOOR / TIRE node translations relative to
their BODY parent, read from the node blocks (`0xffffff72`: name, parent name,
4x4 matrix) of `XMED-6650.bin`. Each part hinges about its node origin, which
is what the damage animation rotates around.

### Damage

Damage (`g_DamagePercent`) accumulates from the original collision pushes. The
damage behaviour is ported from `doParts`, `doAnims`, `doupdate` and
`doDamageMaxOutReset`:

- Panels come loose on their hinges (`partStateList`): bonnet above 20 %, left
  door above 40 %, right door above 60 %, boot above 80 %. Each one swings with
  vertical jolts, yaw and throttle, damped 2 %/frame, and bounces off its
  hinge stops at 60 % speed (bonnet -60..0, doors -80..0, boot 0..60). The
  MeanieCropper boot lid drops down at 3x the angle. The boot also lifts while
  the nitro fires. States 2..4 are empty in the original script, so panels
  swing but never detach.
- Burn smoke (`doSetupBurnSmoke`, Textures member 6 `texture_BurnSmoke_000`)
  pours from 105 units ahead of the car at 60 % damage and above.
- Above 80 % the wheels splay out (rotation.z 40 front, 20 rear).
- At 100 % the car can no longer raise its throttle or accelerate, and the
  "Car Wrecked!" panel (infoPanels 31) shows for 300 ticks. Then
  `doDamageMaxOutReset` repairs it, moves it to `(33650.14, -5.56, 33374.08)`
  in front of Rix's garage, resets the camera, plays Sfx member 24, fades in
  from black (`FadeToBlack.ls`, -2 per frame) and shows panel 32.
- HUD (`Overlays.ls`): damage is capped at 1. The red skull overlay at
  (246, 1) lights to full blend whenever the damage changes and fades 5 per
  frame; at 100 % it stays lit.
- Message panels (`InfoPanels.ls`): drawn at (278, 9), growing from scale 0
  to 1 over 14 frames while up and shrinking/fading when dismissed.

### Ferris' Wheels

`src/ferris.cpp` ports `doSetupFerrisWheel`, `doResetFerrisWheel`,
`doFerrisSwing` and `doFerrisWheel`. The wheel (`FerrisWheel_WHEEL_001` at
(32500, 440, 32500)) appears within 4500 units and turns about Z. Six axles
sit 300 units out at 30 + 60 i degrees, counter-rotate to keep the cages level
and swing on a damped spring. A cage passing the loading point turns its car
to face out. Waiting cars ride 135 below their axle.

- Drive into the bottom cage (|dx| < 100, -360 < dz < 200, damage < 1): the car
  is centred and straightened, then parked once past the wheel's z. Panel 8
  shows and Sfx 21 plays if channel 1 is free.
- Left / Right turn the wheel 60 degrees (eased with a cosine over 180 / 1.4
  frames) to the next occupied cage, with Sfx 22 on channel 2. The selected
  car's damage and boost come back and its info panel (1..6) shows.
- Down reverses out with the selected car; the old one stays in its cage.
- Each time the wheel respawns it rebuilds the cages and re-shows the first
  igNitro bottle.

Ownership (`g_CarsOwnedList`) starts like the original save: only the driven
car (`--car`, default MeanieCropper). Winning a race adds the opponent's car to
the wheel. The scrolling pointer-arrow texture on the Ferris wheel tile is not
animated.

### Races

`src/race.cpp` ports `doSetupRaceFlag` / `doRaceFlag`, `doSetupRacePods` /
`doRacePods` and `doSetupOpponent` / `doOpponent` for all five races. The one
race flag sits at whichever of the five flag positions is nearest; race i is
against car i + 1 (Bitsasushi .. Furore) with the original per-race opponent
acceleration (0.03 .. 0.04) and igNitro bottles. `tools/gen_race_data.py` turns
the MapData fields (members 2..6) into `src/race_data.gen.cpp`: the opponent's
red-dot path and the checkpoint dots that become race pods (race 1: 95 dots /
14 pods, race 2: 252 / 41, race 3: 173 / 30, race 4: 72 / 24, race 5: 477 / 86).
Long fields span several XMED text blocks, which the tool joins.

- Within 5000 units the opponent waits beside the flag. Drive
  into the start box facing the flag: the car stops on the line, Sfx 26
  plays, and the lights count Ready / Set / Go! (panels 18-20, 60 ticks each)
  while the car is held straight. Leaving the box cancels (panel 28).
- On Go! the timer runs (`g_TimerState = -2`, 2 hundredths per tick as in
  Score&Timer.ls), the opponent drives the red dots (target speed = dot
  spacing x 0.04, acceleration 0.03), and the two race igNitro bottles appear.
- Pods: pass within 800 units of the bright pod to move on (Sfx 29); the dim
  one is the pod after it. Each pod's arrow points at the next one (`pointAt`
  turns the model's -Z front to the target). Clearing every pod wins:
  panel 34 and the opponent's car joins the Ferris wheel (panel 44 if
  already owned). 300 ticks
  later the race ends at the Ferris' Wheels spawn with a fade.
- If the opponent reaches its last dot first you lose (Sfx 28): in the
  MeanieCropper it "refuses your pink slip" (panel 39); in any other car it
  takes that car (panels 37 / 38) and you continue in your highest owned car.
- Wrecking the car mid-race fails it (panel 30). Driving back within 4500 of
  the Ferris wheel mid-race abandons it (panel 33).
- The flag pole is solid (radius 120) and the opponent pushes the player
  (radius 160), both with the original damage steps.

Not ported: the points stages, the radar's race pod markers, and opponent
tile collisions (the original has none either; it follows the red dots
through everything).

### igNitro

### igNitro

Ported from `doSetupigNitro`, `doIgNitro` and the blast section of `doAnims`:

- Pickup bottles (`igNitroBottle_001`, rotation (0, 180, 120), scale 2). Free
  drive shows only the first of the ten clones, at (34017.54, 50, 32709.34);
  the race and stage bottle lists belong to modes this build does not have.
  Driving within 150 units collects it: boost refills to 1, Sfx 25 plays, and
  the first pickup shows panel 16 ("IgNitro Pickup ... SHIFT KEY to boost").
- The boot bottle (`mIgNitro`) rides in the boot on the per-car `gIgNitro`
  pivot, 50 units below it. A pickup sets `igNitroRot = 250`, decaying 10 %
  per frame, which flips the bottle up out of the boot and back. The boot lid
  opens to 30 while boosting or while the bottle is flipping (the
  MeanieCropper lid drops at 3x the angle).
- Blast jets (`igNitroBlastJet_000`) sit at the per-car exhaust positions with
  rotation (10, 0, random z). While boosting they fade in 5 blend per frame
  and stretch to scale.z 1; afterwards they fade out and shrink. The script
  never positions car #1's right jet, so the port mirrors the left one.

### Rix Rapid Repairs

`doRixRapidRepairs` runs after the collisions each step (`World::doRixRapidRepairs`).
The `RixGarageDoor_000` shutter sits at the Ferris wheel position +
(1152, -378, 1050) and rotates `90 * open` about X, where
`open = 1 - |65 - (dz - 100)| / 380` (fully open 165 units in front).
Driving in (|dx| < 200, -420 < dz < 60) shows panel 9, snaps the car to the
door's x and eases its rotation to 0 (x 0.95 per frame). Past 210 units
inside, the car is held there, damage and loose panels reset to 0, and Sfx 24
plays once (only if channel 1 last played something else). Moving more than
200 units sideways clears the state and hides the panel.

The panel bitmaps are built by `tools/render_panels.py` from the original
infoPanels text strings and the layout decoded from their style tables: 210 px
text box, 12/16 px character sizes, fixed line spacing (36/18 for "Car
Wrecked!", 16 for the Rix messages), white fill at offset (5, 20). The text
members use the embedded PFR1 font "Speedway BV" (`XMED-6612`), which FreeType
cannot load; `assets/Speedway BV.ttf` is its TrueType conversion and is what
the tool renders with.

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
  bilinear filtering with per-pixel mipmap selection (alpha-weighted mip
  chain built at load), z-buffer, near-plane and frustum culling, back-face
  culling, Director-style lighting (below) and distance fog.
- Lighting follows the original scene and shaders. `tools/gen_materials.py`
  reads the 3D stream's material resources, shader texture layers and lights
  into `src/material_data.gen.cpp`: three coloured directional lights (blue
  fill, warm key, grey) plus a 0.05 ambient, and per shader the ambient,
  diffuse, specular, emissive and shininess. Lighting is per vertex (Gouraud)
  in RGB; a textured shader ignores its diffuse colour
  (`useDiffuseWithTexture` FALSE) and specular is added after texturing. The
  car shells and windows have a second texture layer,
  `BitsasushiTextureReflectionMap_002` (`assets/ReflectionMap.jpg`), in
  reflection mode (sphere-mapped) blended at 0.5, which gives the paint its
  sheen. The packed vertex normals are unreliable, so smooth normals are
  rebuilt from the geometry at load. It renders at the window resolution: `drawMesh`
  transforms (with a per-call vertex cache) and sets up triangles, `flush`
  bins them into 8-row strips and rasterizes the strips on all cores, so
  per-pixel draw order is preserved. The opaque world is drawn nearest tile
  first and the skybox after it, so covered pixels are not shaded twice.
  Framebuffer is uploaded 1:1 to an SFML texture each frame.

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

The pass runs at the same point in the frame as the original. `Car::update()`
takes two hooks and calls them at the original frame positions:

```
doRayCasting (ground hook) -> doControls -> doGravity -> doFriction
  -> doTireGrip -> doCollisions (collision hook) -> doBodyRoll -> doupdate
```

The collision hook runs between `doTireGrip` and `doBodyRoll`, so the pushes act
on the current velocity while the car is still at its pre-move position;
`doupdate` then carries the pushed velocity out of the obstacle. Running
collisions after the move instead makes the pushed velocity re-trigger on the
next frame, which shows up as the car bouncing off walls — that ordering is the
difference.

## Ground contact and the impact wiggle

`rotVector` in the original is an angular **velocity**, not an angle: `doupdate`
does `mPlayerCar.rotate(rotVector * g_FPSFactor, #parent)`, so the body rotation
accumulates frame by frame (`Car::bodyPitchDeg` / `bodyRollDeg`, drawn as the
car's pitch and roll). `Car::rotXDeg` / `rotZDeg` are that angular velocity.

Two things feed it:

- `doBodyRoll`: `diff = velVector - prevVelVector` (horizontal, capped at 1),
  then `rot.x -= diff.z * 0.35`, `rot.z += diff.x * 0.35`. A sudden change in
  velocity — a wall push — injects angular velocity.
- `doRayCasting`: twelve downward rays from `gCageList`. The four wheel points
  (y = 30) get `imbed = isectY - cageY + 30` (floored at 0, capped at 20), an
  upward impulse `vel.y += cap * 0.045`, and a torque
  `rot.x -= cage.z * cap * 0.0008`, `rot.z += cage.x * cap * 0.0008`. The eight
  upper points use `0.0003` plus a velocity/rotation coupling. Anything deeper
  than 20 units becomes `hoist`, added to the position by `doupdate`.

That torque is the spring: when an impact tips the chassis, the wheels on the
low side embed further, the impulse pushes them back up, and the car rocks until
the torque and `0.16`/frame angular damping settle it. On flat ground the
equilibrium is `y = ground - 5.55`, which is exactly the original spawn height
`doSetupPlayerCar` uses (`-5.56`).

The torque uses the cage points in the car's **own** frame (`kCage[i].x/.z`),
not the world-rotated offsets: `rotVector` is applied about the body's pitch and
roll axes, so mixing a world lever arm into a body-frame rotation makes the
suspension unstable at any heading other than the axis-aligned ones.

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

# Let the suspension settle at one spot and report height, body pitch/roll
# and the geometric ground slope under the wheels.
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

# Per-frame suspension trace on stderr (position, rotVector, imbed caps).
RR4_DEBUG_GROUND=1 ./build/rr4 --drive 240 --place 32500 29500 0
```

Results from this checkout:

- Auto drive (car #1): the route covers the docks, Golden Gate bridge,
  Financial district, Cisco Hill, NurdBurger ring and Ferris wheel; screenshots
  are written to `work/final_shots/`.
- Software renderer: native 960 x 600 framebuffer (800 x 480 HUD stage),
  ~9 ms/frame, ~110 fps (`--bench 300`, Ryzen 5 4650U, 12 threads). The old
  single-threaded 640 x 400 upscaled path measured ~22 ms/frame.
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
| body roll `rotVector.x/z` from `vel - prevVel` | `Car::rotXDeg/rotZDeg` |
| `rotVector` is angular velocity, accumulated by `doupdate` | `Car::bodyPitchDeg/bodyRollDeg` |
| cage-ray suspension (`imbed`, `imbedCap`, `hoist`, torques) | `World::updateGround` |
| camera FOV 55, range 320/480, height 120/200, aim +90, `lookingForwards` | `ChaseCam::update` (`doCam`) |
| `tileIDList` / `tileRotList` (144 cells) | `src/world.cpp`, byte-for-byte values |
| 37 tile resource names | `kTileNames` |
| `col = int(x/5000 + 0.5)`, `row = int(z/5000 + 0.5)` | `World::tileColAt/tileRowAt` |

The per-frame update order follows the original `enterFrame`:

```
doRayCasting -> doControls -> doGravity -> doFriction -> doTireGrip
  -> doCollisions -> doBodyRoll -> doupdate -> doAnims -> doCam
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
- No traffic, opponents, race flags or points. The start-up welcome panel
  (infoPanels 10) and the opening fade from black are not shown.
