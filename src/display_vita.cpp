// PS Vita screen: vitaGL owns the 960 x 544 display buffers. It is set up
// once (vglInit cannot be repeated), so a settings change only updates vsync;
// the multisample mode applies from the next start.

#include "display.hpp"
#include "renderer.hpp"

#include <psp2/kernel/sysmem.h>
#include <vitaGL.h>

#include <cstdio>

// newlib heap for the game (world, audio, CPU-side textures). vitaGL takes
// what is left above kSystemReserve for GPU buffers and textures.
extern "C" {
int _newlib_heap_size_user = 128 * 1024 * 1024;
unsigned int sceUserMainThreadStackSize = 2 * 1024 * 1024;
}

namespace rr4 {

namespace {

constexpr int kScreenW = 960, kScreenH = 544;
// RAM vitaGL leaves to the system and other allocations.
constexpr int kSystemReserve = 24 * 1024 * 1024;

SceGxmMultisampleMode multisampleMode(int samples) {
    if (samples >= 4) return SCE_GXM_MULTISAMPLE_4X;
    if (samples >= 2) return SCE_GXM_MULTISAMPLE_2X;
    return SCE_GXM_MULTISAMPLE_NONE;
}

bool gVglReady = false;

void logFreeMemory(const char *when) {
    SceKernelFreeMemorySizeInfo info{};
    info.size = sizeof(info);
    if (sceKernelGetFreeMemorySize(&info) < 0) return;
    std::printf("memory %s: user %d KB, cdram %d KB, phycont %d KB\n", when,
                info.size_user / 1024, info.size_cdram / 1024, info.size_phycont / 1024);
}

} // namespace

std::vector<Resolution> availableResolutions() { return {{kScreenW, kScreenH}}; }

void Display::destroy() {
    // vitaGL stays up until the process ends (vglEnd is not paired with a
    // second vglInit); the display only stops being usable.
    ready_ = false;
}

bool Display::create(const Settings &s, bool useGL) {
    if (!useGL) {
        std::fprintf(stderr, "video: the Vita build only has the vitaGL renderer\n");
        return false;
    }
    if (!gVglReady) {
        logFreeMemory("before vglInit");
        // The return value only says whether the resolution had to fall back
        // (GL_TRUE); vitaGL aborts on its own if it cannot start.
        if (vglInitExtended(0, kScreenW, kScreenH, kSystemReserve, multisampleMode(s.msaa)))
            std::fprintf(stderr, "video: vitaGL fell back to a smaller resolution\n");
        gVglReady = true;
        logFreeMemory("after vglInit");
    }
    vglWaitVblankStart(s.vsync ? GL_TRUE : GL_FALSE);
    gl_ = true;
    ready_ = true;
    return true;
}

void Display::present(Renderer &r) {
    r.flush();
    vglSwapBuffers(GL_FALSE);
}

} // namespace rr4
