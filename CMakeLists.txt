cmake_minimum_required(VERSION 3.16)

# PS Vita: configure with the vitasdk toolchain, e.g.
#   cmake -S . -B build-vita -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake
if(DEFINED ENV{VITASDK} AND CMAKE_TOOLCHAIN_FILE MATCHES "vita")
    set(RR4_VITA ON)
endif()

project(rr4_native LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RR4_SOURCES
    src/main.cpp
    src/input.cpp
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
    src/spritefont.cpp
    src/settings.cpp
    src/keyvalue.cpp
    src/cinematic.cpp
    src/stage_data.gen.cpp
    src/material_data.gen.cpp
)

if(RR4_VITA)
    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

    set(VITA_APP_NAME "Redline Rumble 4")
    set(VITA_TITLEID "RRUM00004")
    set(VITA_VERSION "01.00")
    set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")   # extended RAM

    add_executable(rr4 ${RR4_SOURCES} src/display_vita.cpp)
    target_include_directories(rr4 PRIVATE src $ENV{VITASDK}/arm-vita-eabi/include/SDL2)
    target_compile_options(rr4 PRIVATE -O2 -Wall -Wextra -Wno-unused-parameter
                           -mcpu=cortex-a9 -mfpu=neon)
    target_link_libraries(rr4 PRIVATE
        SDL2_image SDL2 webpdemux webp sharpyuv jpeg png16 z
        vitaGL vitashark SceShaccCgExt SceShaccCg_stub mathneon taihen_stub
        SceKernelDmacMgr_stub SceGxm_stub SceDisplay_stub SceCtrl_stub SceAudio_stub
        SceAudioIn_stub SceSysmodule_stub SceCommonDialog_stub SceAppMgr_stub SceAppUtil_stub
        SceTouch_stub SceHid_stub SceMotion_stub ScePower_stub SceIme_stub SceRazorCapture_stub
        pthread m stdc++)

    vita_create_self(eboot.bin rr4 UNSAFE)
    vita_create_vpk(rr4.vpk ${VITA_TITLEID} eboot.bin
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE vita/sce_sys/icon0.png sce_sys/icon0.png
        FILE vita/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
        FILE vita/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
        FILE vita/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
        FILE assets/rr4_world.bin assets/rr4_world.bin
        FILE assets/ReflectionMap.jpg assets/ReflectionMap.jpg
        FILE assets/hud assets/hud
        FILE assets/sounds assets/sounds
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
    find_package(OpenGL REQUIRED)

    add_executable(rr4 ${RR4_SOURCES} src/display_sdl.cpp)
    target_include_directories(rr4 PRIVATE src)
    target_link_libraries(rr4 PRIVATE PkgConfig::SDL2 PkgConfig::SDL2_IMAGE OpenGL::GL)
    target_compile_options(rr4 PRIVATE -O2 -Wall -Wextra -Wno-unused-parameter)
endif()
