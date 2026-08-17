#include <src/app.hpp>

#include <fstream>


static size_t getFramebufferSizeBytes(Options const& options)
{
    return sizeof(Renderer::Pixel) * options.framebufferWidth * options.framebufferHeight;
}


static riscv::Emulator createEmulator(Options const& options)
{
    std::ifstream file { options.programPath, std::ios::binary };

    if (!file.is_open())
        throw std::runtime_error { "failed to open program file" };

    std::vector<riscv::Byte> program {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(),
    };

    riscv::Size memorySize = options.freeMemorySize + getFramebufferSizeBytes(options);

    return riscv::Emulator { program, memorySize };
}


static Renderer createRenderer(Options const& options)
{
    Renderer renderer { "RISC-V Emulator", options.framebufferWidth, options.framebufferHeight };
    renderer.clear();
    return renderer;
}


static std::span<Renderer::Pixel> getFramebufferSpan(Options const& options,
    riscv::Emulator& emulator)
{
    return std::span {
        reinterpret_cast<Renderer::Pixel *>(emulator.getMemory().data()),
        getFramebufferSizeBytes(options),
    };
}


App::App(std::span<char *> args)
    : options { Options::parse(args) },
      emulator { createEmulator(options) },
      framebuffer { getFramebufferSpan(options, emulator) },
      renderer { createRenderer(options) }
{
}


void App::tick()
{
    renderer.renderFramebuffer(framebuffer);
}

void App::handleKey(SDL_Keycode keyCode, bool isKeyDown)
{
    // TODO How to handle keys?
    (void) keyCode;
    (void) isKeyDown;
}
