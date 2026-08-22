#ifndef RENDERER_HPP
#define RENDERER_HPP


#include <SDL3/SDL.h>

#include <span>
#include <string>


class Renderer
{
public:
    /** This uses a 32-bit little-endian ARGB format.
        The memory layout is as follows: B, G, R, A.
        B is in the lowest byte, A is in the highest byte.
        This matches the endianness of RISC-V (which is little-endian). */
    using Pixel = uint32_t;

    Renderer(std::string const& title, uint32_t framebufferWidth, uint32_t framebufferHeight);

    ~Renderer();

    void presentEmptyFrame();

    void presentFramebuffer(std::span<Pixel> framebuffer);

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
};


#endif
