#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <src/riscv/emulator.hpp>

#include <SDL3/SDL.h>

#include <span>
#include <string>


class Window
{
public:
    Window(std::string const& title, int framebufferWidth, int framebufferHeight);

    ~Window();

    void renderEmpty();

    void renderFramebuffer(std::span<emulator::Pixel> framebuffer);

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    int framebufferWidth;
    int framebufferHeight;
};


#endif
