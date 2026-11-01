cmake_minimum_required(VERSION 3.16)
project(rr4_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(rr4
    src/main.cpp
    src/renderer.cpp
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
    src/material_data.gen.cpp
)

target_include_directories(rr4 PRIVATE src)
target_link_libraries(rr4 PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

target_compile_options(rr4 PRIVATE -O2 -Wall -Wextra -Wno-unused-parameter)
