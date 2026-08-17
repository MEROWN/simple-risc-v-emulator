#define SDL_MAIN_USE_CALLBACKS 1

#include <src/options.hpp>
#include <src/renderer.hpp>
#include <src/riscv/emulator.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <exception>
#include <iostream>
#include <memory>
#include <vector>

std::unique_ptr<Renderer> renderer;

// TODO: remove this and use a raw pointer into the emulator memory instead
std::vector<Renderer::Pixel> fb;

SDL_AppResult SDL_AppInit(void **, int argc, char **argv)
{
    Options options;
    try
    {
        options = Options::parse({ argv, (size_t) argc });
    }
    catch (std::exception const& e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return SDL_APP_FAILURE;
    }

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
        return SDL_APP_FAILURE;

    renderer = std::make_unique<Renderer>("RISC-V Emulator", options.framebufferWidth,
        options.framebufferHeight);

    renderer->clear();

    fb = std::vector<Renderer::Pixel>(options.framebufferWidth * options.framebufferHeight);
    for (size_t i = 0; i < fb.size(); ++i)
        fb[i] = 0xffff7700; // orange

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *)
{
    renderer->renderFramebuffer(fb);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *, SDL_AppResult)
{
    renderer.reset();
}
