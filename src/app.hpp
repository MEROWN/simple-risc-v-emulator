#ifndef APP_HPP
#define APP_HPP


#include <src/options.hpp>
#include <src/renderer.hpp>
#include <src/riscv/emulator.hpp>

#include <SDL3/SDL_keycode.h>

#include <span>
#include <vector>


class App
{
public:
    App(std::span<char *> args);

    void tick();

    void handleKey(SDL_Keycode keyCode, bool isKeyDown);

private:
    Options options;
    riscv::Emulator emulator;
    riscv::Thread mainThread;
    Renderer renderer;

    size_t getFramebufferSize();
    std::span<Renderer::Pixel> getFramebuffer();

    /** Pressed keys are stored as SDL_Keycode codes.
        Released keys are represented as negative values. */
    std::vector<int32_t> keyEvents {};
};


#endif
