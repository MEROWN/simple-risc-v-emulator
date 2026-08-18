#include <src/renderer.hpp>

#include <stdexcept>


Renderer::Renderer(std::string const& title, int framebufferWidth, int framebufferHeight)
    : framebufferWidth { framebufferWidth }, framebufferHeight { framebufferHeight }
{
    window = SDL_CreateWindow(
        title.c_str(), framebufferWidth, framebufferHeight, SDL_WINDOW_RESIZABLE
    );

    if (!window)
        throw std::runtime_error { "failed to create SDL window" };

    SDL_SetWindowMinimumSize(window, framebufferWidth, framebufferHeight);

    renderer = SDL_CreateRenderer(window, nullptr);

    SDL_SetRenderVSync(renderer, true);

    if (!renderer)
        throw std::runtime_error { "failed to create SDL renderer" };

    // SDL_PIXELFORMAT_BGRA32 is ARGB32LE, it does not depend on machine endianness.
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_BGRA32,
        SDL_TEXTUREACCESS_STREAMING,
        framebufferWidth,
        framebufferHeight
    );

    if (!texture)
        throw std::runtime_error { "failed to create SDL texture" };

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

Renderer::~Renderer()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Renderer::clear()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Renderer::renderFramebuffer(std::span<Pixel> framebuffer)
{
    int framebufferPitch = framebufferWidth * (int) sizeof(Pixel);
    SDL_UpdateTexture(texture, nullptr, framebuffer.data(), framebufferPitch);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
