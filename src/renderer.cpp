#include <src/renderer.hpp>

#include <stdexcept>


Renderer::Renderer(std::string const& title, uint32_t framebufferWidth, uint32_t framebufferHeight)
    : framebufferWidth { framebufferWidth }, framebufferHeight { framebufferHeight }
{
    int w = static_cast<int>(framebufferWidth);
    int h = static_cast<int>(framebufferHeight);

    window = SDL_CreateWindow(title.c_str(), w, h, SDL_WINDOW_RESIZABLE);

    if (!window)
        throw std::runtime_error { "failed to create SDL window" };

    SDL_SetWindowMinimumSize(window, w, h);

    renderer = SDL_CreateRenderer(window, nullptr);

    SDL_SetRenderVSync(renderer, true);

    if (!renderer)
        throw std::runtime_error { "failed to create SDL renderer" };

    // SDL_PIXELFORMAT_BGRA32 is ARGB32LE, it does not depend on machine endianness.
    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, w, h
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

void Renderer::presentEmptyFrame()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Renderer::presentFramebuffer(std::span<Pixel> framebuffer)
{
    int framebufferPitch = static_cast<int>(framebufferWidth * sizeof(Pixel));
    SDL_UpdateTexture(texture, nullptr, framebuffer.data(), framebufferPitch);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
