# Redline Rumble 4 - native C++ port

A from-scratch C++20/SDL2 reimplementation of the playable car of
**Redline Rumble 4** (Adobe Director 11.5.9, `redlinerumble4.dcr`), built by
decompiling the original Lingo and using the original Shockwave 3D geometry and
textures.

Scope of this build: **all six original player cars, the whole 12 x 12 map,
the five races, the eight point stages (time attacks), traffic, the airport
plane, igNitro pickups, the Ferris wheel garage, Rix Rapid Repairs, the paint
machine and the radar**, all ported from the decompiled Lingo. The game
autosaves to `rr4save.txt` next to the executable. A new game starts with car #1
(MeanieCropper), the car the original default save owns
(`g_CarsOwnedList = [2,0,0,0,0,0]`); `--car N` drives any of the other five for
that run. The original music and sounds play through the same Director sound
channels.

## Build

Dependencies: CMake >= 3.16, a C++20 compiler, pkg-config, SDL2 and
SDL2_image.

```bash
sudo apt install libsdl2-dev libsdl2-image-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
```

## Run

```bash
./build/rr4              # car #1, MeanieCropper
./build/rr4 --car 4      # any of the six original cars
./build/rr4 --soft       # software rasterizer instead of OpenGL
./build/rr4 --no-shadows --msaa 0   # OpenGL without shadows / anti-aliasing
```

The game opens on a main menu (DRIVE / RESUME, NEW GAME, SETTINGS, QUIT; Up / Down and
Enter, Esc in game returns to it) over a real-time cinematic tour of the
city: six camera shots (Ferris wheel orbit, street-level traffic, the Golden
Gate Bridge, the 747 landing, a flyover and the Nurd Burger ring) on the live
world, with traffic, traffic lights and the plane running. NEW GAME asks for
confirmation and resets `rr4save.txt`. `--place X Z YAW` skips the menu.
Menu text uses the original typeface: `tools/render_menu.py` renders the title
sprites and a font atlas that `src/spritefont.cpp` draws at run time.

SETTINGS: resolution (the display's modes), windowed / fullscreen, vsync,
anti-aliasing (off / 2x / 4x / 8x), shadows, draw distance (2 to 5 tiles
around the camera; the original draws 2), smooth or pixelated textures, music
and sound volume. They are kept in `rr4settings.txt` next to the executable
(`key = value`, like the save). Sound and draw distance apply at once; the
window and renderer are rebuilt when the settings screen closes.

The window renders with OpenGL 2.1 (GLSL 1.20) by default: the same Director
lighting as the software rasterizer, 4x MSAA, anisotropic filtering and
real-time shadows from the key light (a remaster addition; the original only
had baked tile shadow planes, which it disables on software rendering).
Headless modes (`--auto`, `--bench`, ...) use the software rasterizer unless
`--gl` is given, which renders through an offscreen GL context.

The car starts where the original `doSetupPlayerCar` puts it:
`worldPosition = (32053.57, -5.56, 30797.51)`, `rotation.y = 45`, which is the
Ferris wheel tile (`TILE_FerrisWheel_000`, the "FERRIS' WHEELS / UNITED AUTO
SALES" frontage).

Controls:

| Key | Action |
| --- | --- |
| Up / Down | accelerate / brake + reverse |
| Left / Right | steer (also: pick a cage in the Ferris wheel, pick a paint job in the paint machine) |
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
  SDL_image. `src/world.cpp` assembles the map, `src/car.cpp` draws the player car.
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

Not ported: opponent tile collisions (the original has none either; it
follows the red dots through everything).

The race flag disappears when a race starts and returns 5 s after it ends
(a port addition); it cannot be triggered and its pole is not solid while
hidden.

### Point stages (time attacks)

`src/stage.cpp`, from `doSetupPointsFlag`, `doPointsFlag`, `doSetupPointPods`
and `doPointPods`; the flag/pod lists are generated by
`tools/gen_stage_data.py` into `src/stage_data.gen.cpp`.

- The points flag stands at whichever of the eight `pointsFlagPosList` starts
  is nearest. Lining up in its box (as for races) shows Ready / Set / Go
  (panels 18-20, one per second) and loads the countdown: 2, 2, 3, 4, 5, 5, 5
  and 4 minutes for stages 1-8. Leaving the box before Go cancels
  (panel 22).
- Green pods (`PointsPod_001`, next pod dimmed green, mist hidden) are worth
  100 points each within 200 units. Stage 8's last pods follow the landing
  plane, and starting it puts the plane on its approach. Stage 5 raises the
  traffic limit to 20.
- The three stage igNitro bottles are shown during a stage.
- Last pod: panel 21 ("STAGE COMPLETE! TIME BONUS"), the remaining time drains
  into the score (+23 per 1.23 s, `g_TimerState` 2). Time out: panel 25
  ("TIME UP!"). Wrecking the car: panel 29, score reset.
- Like the race flag, the points flag hides during a stage and returns 5 s
  after it ends. Races and stages cannot start while the other is running.
- Not ported: the online score submission (panel 23) and the zoom of the
  score digits during the time bonus.

### Traffic

`src/traffic.cpp`, from `doSetupTraffic`, `doTraffic`, `doSetupTrafficLights`,
`doTrafficLights` and the traffic loop of `doCollisions`. The road network is
MapData member 1 (`doLoadData`), generated into `src/traffic_data.gen.cpp` by
`tools/gen_race_data.py`: 1007 dots with next/previous links, the red and
green traffic-light dots and the dots per tile.

- 20 `TrafficCar_000` clones, at most 5 in the world (20 during stage 5).
  Cars spawn on free dots 4000-4500 from the player in the 3 x 3 tiles around
  them and are removed beyond 4500. They fade in over that band.
- Each dot holds one car (`dotOccupiedList`). A car at its dot picks a random
  next dot and waits while it is taken. Speed eases towards
  `0.02 * distance to the next dot`; steering turns 4.5 degrees per frame
  towards the target, and the body looks back at its previous position, so it
  pitches on slopes. Heights come from the blended height field.
- Traffic lights run the original 6-phase cycle (400/100/100 ticks) and drive
  the `Texture_Tile_TrafficLightPhase1/2*` shader colours. Red light dots are
  held at -1111, so cars stop at them.
- Hitting a car: radius 140 push, damage +0.002 / +0.005 and CrashTinkle
  small / medium.

### Airport: plane and radar dish

`src/plane.cpp`, from `doSetupPlane` / `doPlane`: the Boeing 747
(`TrafficBoeing747_001`) runs the original 16-state loop: approach from
70000 north at height 1000, descent and flare, braking along the runway, taxi
to the stand, a 200-tick wait, push-back, turn, take-off roll, climb, banked
turn east and away off the map, then round again. The plane collider of
`doCollisions` (airport tiles 13-15) follows the plane's real transform.
`AirPortRadar_000` spins 5 degrees per frame (`doAnims`); its collider uses
the same angle.

### Paint machine (Splash of Color)

`src/colortank.cpp`, from `doSetupColorTank` / `doColorTank`. Drive under the
paint cans next to Ferris' Wheels to park (panel 7). Left / Right swing the
cans to the previous / next of the eight paint jobs and pour the splash; the
car is repainted half way through the pour. Reverse to leave. Each car keeps
its own paint job (`g_CarsColorIDList`), also when parked in the Ferris wheel.

### Autosave

`src/savegame.cpp`. The original keeps
`[g_CarsOwnedList, g_CarsColorIDList, g_CarsDamageList, g_CarsIgNitroList]`
with `setPref`; the port writes the same lists to `rr4save.txt` next to the
executable, as `key = values` text:

```
# Redline Rumble 4 save
version = 1
cars_owned = 2 0 0 0 0 0
car_colors = 1 2 3 4 5 1
car_damage = 0.00 0.00 0.00 0.00 0.00 0.00
car_ignitro = 1.00 1.00 1.00 1.00 1.00 1.00
```

It saves where the original calls `doSaveGameData`: winning a car, losing one,
entering or leaving the Ferris wheel, leaving the paint machine and leaving
Rix Rapid Repairs. Unknown keys are ignored and missing keys keep their
defaults; writes go through a temporary file and a rename. On start the
driven car (`2`), the paint jobs and the driven car's damage and igNitro are
restored (the original's loader misspells the damage list, so it never
restores damage; the port does).

### igNitro

Ported from `doSetupigNitro`, `doIgNitro` and the blast section of `doAnims`:

- Pickup bottles (`igNitroBottle_001`, rotation (0, 180, 120), scale 2). Free
  drive shows only the first of the ten clones, at (34017.54, 50, 32709.34);
  races and stages show their own bottles from the same ten clones.
  Driving within 150 units collects it: boost refills to 1, Sfx 25 plays, and
  the first pickup shows panel 16 ("IgNitro Pickup ... SHIFT KEY to boost").
- The boot bottle (`mIgNitro`) rides in the boot on the per-car `gIgNitro`
  pivot, 50 units below it. A pickup sets `igNitroRot = 250`, decaying 10 %
  per frame, which flips the bottle up out of the boot and back. The boot lid
  opens to 30 while boosting or while the bottle is flipping (the
  MeanieCropper lid drops at 3x the angle).
- Blast jets (`igNitroBlastJet_000`) sit at the per-car exhaust positions with
  rotation (10, 0, random z). While boosting they fade in 5 blend per frame
  and stretch to scale.z 1; afterwards they fade out and shrink. They start at
  blend 100, so they fade out on the first frames. The script sets car #1's
  left jet position twice and never the right one, so car #1 shows a single
  jet (the right jet keeps the previous car's position after a car swap).
  The ignition sound plays whenever the boost starts, in the air too.

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
| radar blips: Rix garage, next race flag, points flag | 55 / 56 / 57 | (44, 424) + direction * min(0.005 * distance, 32) | `doRadar()` |
| radar blips: current stage / race pod | 58 / 59 | (48, 428) + direction * min(0.01 * distance, 32) | `doRadar()` |
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

`src/audio.*` reproduces the Director sound channels used by the car. It
decodes the original WAV files with SDL2 and mixes five independent voices in
an SDL audio callback (one each for music, engine, brake/reverse, skid and
one-shots):

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
  cigarette and the airport radar (spinning, see "Airport").
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
  culling, Director-style lighting (below) and distance fog. The fog (a port
  addition; the original has none) follows the draw distance setting: it is
  solid 500 units before the nearest undrawn tile and starts at a quarter of
  that, so tiles and props never pop in.
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
  sheen. The packed vertex normals are unreliable, so normals are rebuilt from
  the geometry at load: corners at the same position average the faces that
  bend less than 60 degrees from their own, which smooths the car shells
  (whose triangles do not share vertices) and keeps hard edges. It renders at the window resolution: `drawMesh`
  transforms (with a per-call vertex cache) and sets up triangles, `flush`
  bins them into 8-row strips and rasterizes the strips on all cores, so
  per-pixel draw order is preserved. The opaque world is drawn nearest tile
  first and the skybox after it, so covered pixels are not shaded twice.
  Framebuffer is uploaded 1:1 to an SDL texture each frame.

## Performance and memory

Desktop timings at 960x600 (`--bench 60`): about 22-24 ms/frame on the software
rasterizer and 8-9 ms/frame through OpenGL with a readback, on a 12-thread
machine. The default window renders on the GPU; `--soft` is the CPU fallback.

`tools/bench_memory.sh` runs every mode under GNU time and prints peak resident
memory. Current numbers on this desktop:

| mode | peak RSS |
| --- | --- |
| assets only (`--trace`) | 8.4 MB |
| probe (`--probe`) | 24.4 MB |
| audio (`--audio-test`) | 30.6 MB |
| software bench | 38.0 MB |
| OpenGL bench | 86.6 MB |
| interactive software | 111.7 MB |
| interactive OpenGL | 96.5 MB |

Most of the interactive figure is the SDL/X11/GL driver, which does not exist
on a console. The game's own retained memory is roughly: ~16 MB world (asset
meshes, textures, height fields), ~22 MB decoded audio, plus framebuffers and
HUD textures. Two changes keep it there:

- Tile batches reference the loaded `AssetMesh` groups (`indexBegin`/
  `indexCount`) instead of copying the whole vertex array per material; the
  copies cost about 20 MB because a tile shares one mesh between several
  materials.
- Audio clips stay mono in RAM (the original Sfx are mono), so the 19 MB of WAV
  data is not doubled into stereo; the mixer writes each sample to both output
  channels.

For the Vita (512 MB, 960x544) that is comfortable. A PSP (32 MB) is not:
the world and audio alone exceed it, so PSP would need streamed/compressed
audio, quantized or sparse height fields and smaller textures before it fits.

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

# Preview the main menu's cinematic shots (a frame every 300 ticks).
./build/rr4 --gl --menu-preview 3600 --shot-every 300 --shots work/menu

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
src/renderer.*      Renderer interface, software rasterizer (SoftRenderer)
src/gl_renderer.*   OpenGL backend: recorded draws, shadow map, MSAA
src/world.*         original tile map, props, height field
src/collision.*     doRadialPush() and the four-tile collision pass
src/collision_cases.gen.cpp  per-tile clamps generated from the Lingo
src/garage.hpp      car count, paint jobs, ownership states and defaults
src/ferris.*        Ferris' Wheels garage (store and swap cars)
src/race.*          the five races (flag, lights, opponent, pods)
src/stage.*         the eight point stages / time attacks
src/traffic.*       traffic cars and traffic lights
src/plane.*         the airport Boeing 747 loop
src/colortank.*     the Splash of Color paint machine
src/savegame.*      rr4save.txt autosave
src/menu.*          main menu and settings screen over the live city
src/spritefont.*    runtime text from the font atlas
src/settings.*      rr4settings.txt
src/keyvalue.*      shared key = value file reading / atomic writing
src/cinematic.*     cinematic camera shots behind the menu
src/*_data.gen.cpp  race paths, traffic network, stage lists, shader lighting
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
tools/gen_race_data.py      MapData race paths and traffic network
tools/gen_stage_data.py     point stage flags and pods from PlayerCar.ls
tools/gen_materials.py      shader lighting, scene lights, reflection map
tools/render_panels.py      render the InfoPanels message panels with the TTF
tools/render_menu.py        render the main menu text sprites
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

- Canal water is treated as drivable ground, so the autopilot can cut a
  corner and end up in the canal.
- The original twelve-ray cage ground test is approximated by a 128 x 128
  per-tile height field, blended between cells. Steep tree-covered hillsides
  can still put the view close to foliage, and kerbs become short ramps.
- The welcome panel (infoPanels 10), the opening fade from black, the menus
  and online score / time submission are not ported.
- Traffic does not avoid the player and ignores bridges' upper decks (as in
  the original, it follows the dots at their height).
