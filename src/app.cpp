#include <src/app.hpp>

#include <src/options.hpp>

#include <fstream>


static std::vector<uint8_t> readProgramFile(std::string const& path)
{
    std::ifstream file { path, std::ios::binary };

    if (!file.is_open())
        throw std::runtime_error { "failed to open program file" };

    return std::vector<uint8_t> {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(),
    };
}


App::App(std::span<char *> args)
    : options { Options::parse(args) },
      emulator {},
      mainThread {},
      renderer { "RISC-V Emulator", options.framebufferWidth, options.framebufferHeight }
{
    emulator.loadInstructions(readProgramFile(options.programPath));
    emulator.resizeMemory(getFramebufferSize() + options.freeMemorySize);

    renderer.presentEmptyFrame();
}


size_t App::getFramebufferSize()
{
    return sizeof(Renderer::Pixel) * options.framebufferWidth * options.framebufferHeight;
}


std::span<Renderer::Pixel> App::getFramebuffer()
{
    return std::span<Renderer::Pixel> {
        reinterpret_cast<Renderer::Pixel *>(emulator.getMemory().data()),
        getFramebufferSize(),
    };
}


void App::tick()
{
    emulator.run(mainThread);
    renderer.presentFramebuffer(getFramebuffer());
}

void App::handleKey(SDL_Keycode keyCode, bool isKeyDown)
{
    // TODO handle key events
}
