#include <src/app.hpp>


static size_t getFramebufferSizeBytes(Options const& options)
{
    return sizeof(Renderer::Pixel) * options.framebufferWidth * options.framebufferHeight;
}


static riscv::Program loadProgram(Options const& options)
{
    // TODO load instructions
    return std::span<riscv::Instruction> { (riscv::Instruction *) nullptr, 0 };
}


static Renderer createRenderer(Options const& options)
{
    Renderer renderer { "RISC-V Emulator", options.framebufferWidth, options.framebufferHeight };
    renderer.clear();
    return renderer;
}


static riscv::Emulator createEmulator(Options const& options, riscv::Program program)
{
    riscv::Size memorySize = options.freeMemorySize + getFramebufferSizeBytes(options);

    riscv::Emulator emulator { program, memorySize };
    return emulator;
}


static std::span<Renderer::Pixel> getFramebufferSpan(riscv::Emulator& emulator,
    Options const& options)
{
    auto memorySpan = emulator.getMemory();

    return std::span {
        reinterpret_cast<Renderer::Pixel *>(memorySpan.data()),
        getFramebufferSizeBytes(options),
    };
}


App::App(std::span<char *> args)
    : options(Options::parse(args)),
      program(loadProgram(options)),
      renderer(createRenderer(options)),
      emulator(createEmulator(options, program)),
      framebuffer(getFramebufferSpan(emulator, options))
{
}


void App::tick()
{
    renderer.renderFramebuffer(framebuffer);
}

void App::handleKey(SDL_Keycode keyCode, bool isKeyDown)
{
    // TODO How to handle keys?
}
