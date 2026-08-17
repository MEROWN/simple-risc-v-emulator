#include <src/app.hpp>

#include <fstream>


static size_t getFramebufferSizeBytes(Options const& options)
{
    return sizeof(Renderer::Pixel) * options.framebufferWidth * options.framebufferHeight;
}


static std::vector<riscv::Instruction> loadProgram(Options const& options)
{
    std::ifstream file { options.programPath, std::ios::binary | std::ios::ate };

    if (!file.is_open())
        throw std::runtime_error { "failed to open program file" };

    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize % sizeof(riscv::Instruction) != 0)
        throw std::runtime_error { "program file size is not a multiple of instruction size" };

    size_t instructionCount = fileSize / sizeof(riscv::Instruction);
    std::vector<riscv::Instruction> instructions(instructionCount);

    file.read(reinterpret_cast<char *>(instructions.data()), (std::streamsize) fileSize);

    return instructions;
}


static Renderer createRenderer(Options const& options)
{
    Renderer renderer { "RISC-V Emulator", options.framebufferWidth, options.framebufferHeight };
    renderer.clear();
    return renderer;
}


static riscv::Emulator createEmulator(Options const& options, std::span<riscv::Instruction> program)
{
    riscv::Size memorySize = options.freeMemorySize + getFramebufferSizeBytes(options);

    riscv::Emulator emulator { program, memorySize };
    return emulator;
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
      program { loadProgram(options) },
      renderer { createRenderer(options) },
      emulator { createEmulator(options, program) },
      framebuffer { getFramebufferSpan(options, emulator) }
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
