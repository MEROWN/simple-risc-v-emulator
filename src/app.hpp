#ifndef APP_HPP
#define APP_HPP


#include <src/options.hpp>
#include <src/renderer.hpp>
#include <src/riscv/emulator.hpp>

#include <SDL3/SDL_keycode.h>

#include <span>


class App
{
public:
    App(std::span<char *> args);

    void tick();

    void handleKey(SDL_Keycode keyCode, bool isKeyDown);

private:
    Options options;
    riscv::Emulator emulator;
    std::span<Renderer::Pixel> framebuffer;
    Renderer renderer;
};


#endif
