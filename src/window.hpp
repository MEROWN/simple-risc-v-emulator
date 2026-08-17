#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <SDL3/SDL.h>

#include <cstdint>
#include <span>
#include <string>


class Window
{
public:
    /** ARGB32LE: ARGB 32-bit little-endian pixel format.
        The memory layout is as follows: B, G, R, A. */
    using Pixel = uint32_t;

    Window(std::string const& title, int framebufferWidth, int framebufferHeight);

    ~Window();

    void renderEmpty();

    void renderFramebuffer(std::span<Pixel> framebuffer);

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    int framebufferWidth;
    int framebufferHeight;
};


#endif
