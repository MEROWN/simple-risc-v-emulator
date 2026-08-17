#define SDL_MAIN_USE_CALLBACKS 1

#include <src/app.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <exception>
#include <iostream>
#include <memory>


std::unique_ptr<App> app;


SDL_AppResult SDL_AppInit(void **, int argc, char **argv)
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
        return SDL_APP_FAILURE;

    try
    {
        app = std::make_unique<App>(std::span { argv, (size_t) argc });
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error on startup: " << e.what() << std::endl;
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *)
{
    app->tick();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;

    case SDL_EVENT_KEY_DOWN:
        app->handleKey(event->key.key, true);
        break;

    case SDL_EVENT_KEY_UP:
        app->handleKey(event->key.key, false);
        break;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *, SDL_AppResult)
{
    app.reset();
}
