# TODO

## Done

- Main menu over a live cinematic city, NEW GAME, settings screen.
- Settings: resolution, fullscreen, vsync, anti-aliasing, shadows, draw/fog
  distance, texture filtering, music and sound volume (`rr4settings.txt`).
- GPU renderer (OpenGL 2.1) with shadow maps; software renderer kept as reference.
- Autosave (`rr4save.txt`).

## Still missing from the original

- Pause screen, instructions screens, welcome panel, opening fade
  (`MenuMain.ls`, `MenuLevels.ls`, `MenuInstructions.ls`).
- Local high scores and best times (`g_MissionHiScoreList`,
  `g_RaceBestTimeList`) in the save file instead of the dead "Submit score?" step.
- Awards from the original counters (`g_NumberRacesWon`,
  `g_NumberCarsUnlocked`, `g_RacesNoDamage`, `g_MidAirTime`).
- Baked tile shadows (currently skipped).

## Settings still to add

- Field of view.
- Rebindable keys.
- "Original look" toggle: no fog, no fade-ins, original draw distance.
- Widescreen HUD: keep the 800 x 480 layout, anchor it to the screen corners
  (the HUD currently stretches on non-5:3 windows).

## Features (recommended order)

1. **Gamepad + rumble.** Analog steer/throttle/brake through SDL2 game controller,
   rumble on crashes and igNitro. Needed for the Vita anyway; `CarInput` already
   separates input.
2. **Camera modes.** Chase / hood / bumper / cockpit, cycled with `C`. The car
   models have no interior, so the cockpit needs a small dash + steering-wheel
   mesh drawn in camera space; the wheel turns with the steer value. Head sway
   from suspension and G-forces, FOV kick on igNitro.
3. **Instant replay + ghosts.** Ring buffer of car (and traffic) transforms per
   tick; replay with the cinematic camera plus trackside cameras. Best lap per
   race / time attack saved to a file and drawn as a translucent ghost.
   Record transforms, not inputs: an input replay would need a full world snapshot.
4. **Day/night cycle.** Animate the scene lights, sky gradient and fog colour;
   moving sun for the shadow map. Night: headlight cones, lit windows, street
   light sprites.
5. **Photo mode.** Freeze the sim, free camera, hide HUD, FOV, save PNG via
   `readPixels`.
6. **Stunt and near-miss scoring.** Air time, flips, rolls, near misses,
   wrong-way, drifts; combos feed the original score counter.
7. **Police chases.** Wanted level from hitting traffic / running red lights;
   cop cars with pursuit steering reuse the traffic AI.
8. **Slow-mo crash cam.** Quarter-speed sim with an orbiting camera on big
   impacts. Needs render interpolation between fixed steps (also helps on
   120/144 Hz screens).
9. **GPU post-processing.** Bloom, motion blur at speed, colour grading (hook
   into the existing FBO resolve / screen shader).
10. **Split-screen.** Two cars, two viewports. Needs `World` to support more
    than one player car.
11. **Weather.** Rain, wet-road reflections (reflection map exists), lower grip.
12. **In-car radio.** Several music stations on a key.
13. Speedometer, free-roam respawn button, better smoke/nitro/dust, upscaled
    textures (originals are 8x8 to 128x128).

## Crazy ideas

- **Infinite procedural city.** The map is already a grid of modular tiles
  (`kTileID` / `kTileRot`) with per-tile collision cases, so generate the grid
  instead of reading it: wave function collapse over tile edge types (road /
  no road per side), streamed in chunks around the car. Work needed:
  - edge connectivity table for every tile id + rotation;
  - per-tile traffic dot templates (the traffic network is currently global);
  - per-tile height fields and props (streamed, seeded by chunk coordinates);
  - floating origin so float precision holds far from (0, 0);
  - landmarks (Ferris wheel, Nurd Burger, bridge) placed as rare "special" tiles;
  - generated races / time attacks along random routes.
- **Online multiplayer.** Free roam, races, tag. Snapshot-interpolation
  netcode; the 60 Hz fixed step helps.
- **Level editor / mods.** Paint tiles and props on the grid, save maps and
  races to text files in the same `key = value` format.
- **Car combat / derby mode.** Weapons pickups (reuse igNitro pickup code),
  the existing damage, loose parts and wreck/respawn logic.
- **Car soccer.** A big physics ball in the Nurd Burger ring.
- **Physics toys.** Moon-gravity mode, tiny/giant car mode, ramps and loops,
  tornado that throws traffic around, a 747 that can crash.
- **Destructible props.** Lamp posts, signs and benches that break off and
  tumble (the loose car-part code already does rigid tumbling).
- **Tron mode.** Light-trail walls behind cars at night.
- **VR.** Cockpit camera + OpenXR.
- **Learned AI opponents.** Train a small network on the deterministic sim,
  headless at thousands of ticks per second.

## PS Vita port

Doable once the GPU renderer exists (done).

Hardware: quad-core ARM Cortex-A9 up to 444 MHz (3 cores for games),
PowerVR SGX543MP4+, 512 MB RAM, 960 x 544 screen.

1. Renderer: vitaGL (GL ES 2 / GL1-style). Port `GLRenderer` (GL 2.1 / GLSL
   1.20 maps closely to ES 2); smaller shadow map or none.
2. Platform layer: desktop is already on SDL2 (window, input, image loading and
   a small SDL audio mixer; VitaSDK provides SDL2).
3. Platform details: saves in `ux0:data/rr4/`, assets packed in the VPK,
   gamepad controls (triggers throttle/brake, stick steering, face button nitro).
4. Performance: suspension rays (12 per frame, 4 height lookups each), traffic,
   draw calls in the tile window (batch per tile). Keep the 60 Hz step; render
   at 30 fps if needed.
5. Legal: geometry, textures, sounds and music are from the original game.
   Fine for personal use; publishing needs the rights holder's permission.

Memory (see README "Performance and memory"): desktop peak is ~112 MB with the
software renderer and ~97 MB on OpenGL, of which ~60 MB is the X11/GL driver.
The port itself is ~16 MB world + ~22 MB decoded audio + framebuffers, so the
Vita's 512 MB is not a constraint. A PSP (32 MB) would need streamed audio,
quantized/sparse height fields and reduced textures first.

`tools/bench_memory.sh` measures every mode with GNU time and is the number to
watch for regressions.
