# CLAUDE.md

C++20 / SDL2 port of Redline Rumble 4 (Adobe Director Shockwave 3D game,
`redlinerumble4.dcr`) with its own multithreaded software rasterizer. See
README.md for the full feature list and how each system maps to the original.

## Golden rule: copy the original

Behaviour must match the decompiled Lingo, not be invented. Before changing a
gameplay system, read the original handler and port it:

- Main script: `work/dump/redlinerumble4/casts/Internal/BehaviorScript 4 - PlayerCar.ls`
  (`enterFrame` lists the per-frame handler order; `doSetupX` / `doX` pairs).
- HUD and messages: `BehaviorScript 10 - InfoPanels.ls`, `18 - Score&Timer.ls`,
  `20 - Overlays.ls`. Startup / save defaults: `MovieScript 1 - Initialization.ls`.
- Raw Director chunks: `work/dump2/redlinerumble4/chunks/` (the 3D stream is
  `XMED-6650.bin`; text members are `XMED-71xx.bin`).
- Keep the original's quirks when they are visible behaviour (e.g. car #1 has
  one igNitro jet); note deliberate deviations in comments and README.md.
- Comments name the original handler or variable a piece of code comes from
  (`// doColorTank`, `// g_CarsOwnedList`).

Reading tip: tool output may elide repeated lines of the `.ls` files. Print a
handler on one line instead, e.g.

```bash
python3 -c "
L=open('work/dump/redlinerumble4/casts/Internal/BehaviorScript 4 - PlayerCar.ls',encoding='latin1').read().split('\n')
s=[i for i,l in enumerate(L) if l.startswith('on doPlane')][0]; e=s
while not L[e].startswith('end'): e+=1
print(' ;; '.join(L[k].strip() for k in range(s,e+1)))"
```

## Director conventions used by the port

- 60 ticks per second; one fixed step per tick, `g_FPSFactor` = 1.
- `transform.rotation = (x, y, z)` applies X, then Y, then Z
  (`Mat4::rotationZ * rotationY * rotationX`).
- `pointAt` turns a model's -Z towards the target: `pointAtYaw()` in `math.hpp`.
- `textureTransform.position (u, v)`: the port flips V, so pass `du = -u`,
  `dv = v` to `Renderer::setUVOffset`.
- Mesh groups follow the shader list order: group 0 is `shaderList[1]`.
- Car shells are mirrored with scale (-1, 1, 1); the front-right tyre uses
  `-steer`.
- HUD positions are in the original 800 x 480 stage pixels.

## Build and run

```bash
sudo apt install libsdl2-dev libsdl2-image-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j16
./build/rr4 [--car N]
```

PS Vita: `cmake -S . -B build-vita -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake`
then `cmake --build build-vita` (VPK in `build-vita/rr4.vpk`). Platform code is
limited to `src/display_sdl.cpp` / `src/display_vita.cpp`, `#ifdef __vita__`
blocks in `gl_renderer.cpp`, `settings.cpp`, `keyvalue.cpp` and `main.cpp`.

New `.cpp` files must be added to `RR4_SOURCES` in `CMakeLists.txt`. Headless modes
(`--auto`, `--trace`, `--cam-trace`, `--bench`, `--drive`, `--probe`) are listed
in README.md; `tools/verify_physics.py` and `tools/verify_camera.py` check the
car step and camera against independent models of the Lingo.
`tools/bench_memory.sh` reports peak RSS per mode (Vita/PSP budget).

## Code layout

- `src/main.cpp`: game loop, menu/game screens,
  fixed-step loop, headless modes, autosave. `src/display_*.cpp`: the screen (SDL2
  window / GL context, or vitaGL). `src/input.*`: keyboard + game controller. `src/menu.*` + `src/cinematic.*`:
  main menu over the live city (`World::updateAmbient` runs traffic/plane
  without a player; `World::newSession` resets gameplay systems for NEW GAME).
- `src/car.*`: player car physics, parts, damage, igNitro, request flags
  (`panels`, `sfx`, `timerState`, `saveGame`, ...) that HUD/audio/save read
  after each step.
- `src/world.*`: tiles, props, height field. Each step calls
  `World::doCollisions` (traffic + traffic lights, traffic push, tile pushes)
  then `World::doWorldObjects` (opponent, Ferris wheel, plane, Rix repairs,
  paint machine, igNitro pickups, race flag, point stage, radar dish spin),
  following the original `enterFrame` order.
- `src/hud.*` (overlays, panels, timer, score, radar), `src/audio.*`,
  `src/assets.*`.
- Rendering goes through the abstract `Renderer` (`src/renderer.hpp`) with
  two backends: `SoftRenderer` (reference, headless default) and
  `GLRenderer` (`src/gl_renderer.*`, window default; `--soft` / `--gl`).
  Keep their output matching: lighting changes go in both. GL caches meshes
  and textures by their vertex / pixel buffer address, so build meshes once
  (no per-frame temporary `Mesh` objects; use a static mesh plus a matrix).
- `src/garage.hpp`: car count, paint jobs, ownership states, defaults.
- `src/settings.*`: `rr4settings.txt` (video, draw distance, volumes); main
  applies it (`buildDisplay` rebuilds window + renderer on video changes).
  Both files use `src/keyvalue.*`.
- `src/savegame.*`: `rr4save.txt` next to the executable (`key = values`).

## Generated code and assets

Do not edit `src/*.gen.cpp` or `assets/hud/*.png` by hand; regenerate:

| Output | Tool |
| --- | --- |
| `src/collision_cases.gen.cpp` | `tools/gen_collisions.py` |
| `src/race_data.gen.cpp`, `src/traffic_data.gen.cpp` | `tools/gen_race_data.py` |
| `src/stage_data.gen.cpp` | `tools/gen_stage_data.py` |
| `src/material_data.gen.cpp`, `assets/ReflectionMap.jpg` | `tools/gen_materials.py` |
| `assets/hud/*` bitmaps (cast slots in `SLOTS`) | `tools/extract_hud.py` |
| `assets/hud/menu/*.png` | `tools/render_menu.py` |
| `assets/hud/PanelN.png` (ids in `MESSAGES`) | `tools/render_panels.py` (uses `assets/Speedway BV.ttf`) |
| `assets/sounds/*.wav` | `tools/extract_sounds.py` |
| `assets/rr4_world.bin` textures (after editing `assets/textures/tex-N`) | `tools/pack_textures.py` |
| `vita/sce_sys/*` (Vita bubble / LiveArea) | `tools/make_livearea.py` |

## Style

- No magic numbers in new code: name constants (`constexpr`, enums) and say
  where the value comes from in the original.
- Fix root causes; prefer extending the owning module over patches elsewhere.
- Keep the 60 Hz step deterministic (seeded xorshift, not `rand()`).
- Do not commit unless asked.
