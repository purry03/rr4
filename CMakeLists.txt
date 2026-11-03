cmake_minimum_required(VERSION 3.16)
project(rr4_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
find_package(OpenGL REQUIRED)

add_executable(rr4
    src/main.cpp
    src/renderer.cpp
    src/gl_renderer.cpp
    src/assets.cpp
    src/world.cpp
    src/car.cpp
    src/audio.cpp
    src/hud.cpp
    src/collision.cpp
    src/collision_cases.gen.cpp
    src/font.cpp
    src/ferris.cpp
    src/race.cpp
    src/race_data.gen.cpp
    src/traffic.cpp
    src/traffic_data.gen.cpp
    src/plane.cpp
    src/stage.cpp
    src/colortank.cpp
    src/savegame.cpp
    src/menu.cpp
    src/gamepad.cpp
    src/spritefont.cpp
    src/settings.cpp
    src/keyvalue.cpp
    src/cinematic.cpp
    src/stage_data.gen.cpp
    src/material_data.gen.cpp
)

target_include_directories(rr4 PRIVATE src)
target_link_libraries(rr4 PRIVATE PkgConfig::SDL2 PkgConfig::SDL2_IMAGE OpenGL::GL)

target_compile_options(rr4 PRIVATE -O2 -Wall -Wextra -Wno-unused-parameter)
