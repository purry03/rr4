#pragma once
// The game screen: the desktop SDL2 window (OpenGL context, or an SDL
// renderer that shows the software framebuffer) or the PS Vita display
// through vitaGL. One implementation is compiled per platform:
// src/display_sdl.cpp or src/display_vita.cpp.

#include "settings.hpp"

#include <vector>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace rr4 {

class Renderer;

class Display {
public:
    Display() = default;
    Display(const Display &) = delete;
    Display &operator=(const Display &) = delete;
    ~Display() { destroy(); }

    // (Re)creates the screen for these settings; false on failure.
    bool create(const Settings &s, bool useGL);
    void destroy();
    explicit operator bool() const { return ready_; }

    // Shows the finished frame (the renderer is flushed first).
    void present(Renderer &r);

private:
    bool ready_ = false;
    bool gl_ = false;
#ifndef __vita__
    SDL_Window *window_ = nullptr;
    void *glContext_ = nullptr;             // SDL_GLContext
    SDL_Renderer *sdlRenderer_ = nullptr;   // software renderer presentation
    SDL_Texture *screenTexture_ = nullptr;
#endif
};

// Screen sizes to offer in the settings menu, smallest first.
std::vector<Resolution> availableResolutions();

#ifndef __vita__
// Desktop helpers shared with the headless GL modes in main.cpp.
bool ensureVideo();
void setGLAttributes();
#endif

} // namespace rr4
