// Desktop screen: an SDL2 window with an OpenGL 2.1 context (GLRenderer) or
// an SDL renderer that shows the software framebuffer (SoftRenderer).

#include "display.hpp"
#include "gl_renderer.hpp"
#include "renderer.hpp"

#include <SDL.h>

#include <algorithm>
#include <cstdio>

namespace rr4 {

namespace {

// Fallback when the display reports no modes: the software renderer size.
constexpr int kFallbackWidth = 960, kFallbackHeight = 600;

bool displayModeSupported(int w, int h) {
    const int count = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < count; ++i) {
        SDL_DisplayMode mode{};
        if (SDL_GetDisplayMode(0, i, &mode) == 0 && mode.w == w && mode.h == h) return true;
    }
    return false;
}

} // namespace

bool ensureVideo() {
    if (SDL_WasInit(SDL_INIT_VIDEO)) return true;
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "video: SDL init failed (%s)\n", SDL_GetError());
        return false;
    }
    return true;
}

void setGLAttributes() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

// Window sizes to offer: the display's fullscreen modes (one per size, at
// least the original 800 x 480 stage), smallest first.
std::vector<Resolution> availableResolutions() {
    std::vector<Resolution> out;
    ensureVideo();
    auto add = [&](int w, int h) {
        if (w < Settings::kMinWidth || h < Settings::kMinHeight) return;
        if (std::none_of(out.begin(), out.end(),
                         [&](const Resolution &r) { return r.w == w && r.h == h; }))
            out.push_back({w, h});
    };
    const int count = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < count; ++i) {
        SDL_DisplayMode mode{};
        if (SDL_GetDisplayMode(0, i, &mode) == 0) add(mode.w, mode.h);
    }
    SDL_DisplayMode desktop{};
    if (SDL_GetDesktopDisplayMode(0, &desktop) == 0) add(desktop.w, desktop.h);
    std::sort(out.begin(), out.end(), [](const Resolution &a, const Resolution &b) {
        return a.w != b.w ? a.w < b.w : a.h < b.h;
    });
    if (out.empty()) out.push_back({kFallbackWidth, kFallbackHeight});
    return out;
}

void Display::destroy() {
    if (screenTexture_) SDL_DestroyTexture(screenTexture_);
    if (sdlRenderer_) SDL_DestroyRenderer(sdlRenderer_);
    if (glContext_) SDL_GL_DeleteContext(glContext_);
    if (window_) SDL_DestroyWindow(window_);
    screenTexture_ = nullptr;
    sdlRenderer_ = nullptr;
    glContext_ = nullptr;
    window_ = nullptr;
    ready_ = false;
}

bool Display::create(const Settings &s, bool useGL) {
    destroy();
    if (!ensureVideo()) return false;
    gl_ = useGL;

    int w = s.width, h = s.height;
    // A fullscreen size the display cannot do falls back to the desktop.
    if (s.fullscreen && !displayModeSupported(w, h)) {
        SDL_DisplayMode desktop{};
        if (SDL_GetDesktopDisplayMode(0, &desktop) == 0) {
            w = desktop.w;
            h = desktop.h;
        }
    }

    Uint32 flags = SDL_WINDOW_SHOWN;
    if (s.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (useGL) {
        setGLAttributes();
        flags |= SDL_WINDOW_OPENGL;
    }
    window_ = SDL_CreateWindow("Redline Rumble 4", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, w, h, flags);
    if (!window_) {
        std::fprintf(stderr, "video: cannot create window (%s)\n", SDL_GetError());
        return false;
    }

    if (useGL) {
        glContext_ = SDL_GL_CreateContext(window_);
        if (!glContext_) {
            std::fprintf(stderr, "gl: cannot create context (%s)\n", SDL_GetError());
            destroy();
            return false;
        }
        SDL_GL_MakeCurrent(window_, glContext_);
        SDL_GL_SetSwapInterval(s.vsync ? 1 : 0);
        ready_ = true;
        return true;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, s.smoothTextures ? "linear" : "nearest");
    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    if (s.vsync) rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    sdlRenderer_ = SDL_CreateRenderer(window_, -1, rendererFlags);
    if (!sdlRenderer_) sdlRenderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    if (!sdlRenderer_) {
        std::fprintf(stderr, "video: cannot create renderer (%s)\n", SDL_GetError());
        destroy();
        return false;
    }
    screenTexture_ = SDL_CreateTexture(sdlRenderer_, SDL_PIXELFORMAT_ABGR8888,
                                       SDL_TEXTUREACCESS_STREAMING, s.width, s.height);
    if (!screenTexture_) {
        std::fprintf(stderr, "video: cannot create screen texture (%s)\n", SDL_GetError());
        destroy();
        return false;
    }
    SDL_SetTextureBlendMode(screenTexture_, SDL_BLENDMODE_NONE);
    ready_ = true;
    return true;
}

void Display::present(Renderer &r) {
    r.flush();
    if (gl_) {
        int drawableW = 0, drawableH = 0;
        SDL_GL_GetDrawableSize(window_, &drawableW, &drawableH);
        static_cast<GLRenderer &>(r).present(drawableW, drawableH);
        SDL_GL_SwapWindow(window_);
        return;
    }
    auto &soft = static_cast<SoftRenderer &>(r);
    SDL_UpdateTexture(screenTexture_, nullptr, soft.color.data(),
                      soft.width * int(sizeof(uint32_t)));
    SDL_RenderClear(sdlRenderer_);
    SDL_RenderCopy(sdlRenderer_, screenTexture_, nullptr, nullptr);
    SDL_RenderPresent(sdlRenderer_);
}

} // namespace rr4
