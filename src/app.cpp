#include "app.hpp"

#include <src/options.hpp>
#include <src/riscv/elf.hpp>

#include <fstream>


enum class CustomSyscall : uint64_t
{
    /** Returns:
        - Argument0: a random 64-bit integer */
    Random = static_cast<uint64_t>(riscv::BuiltinSyscall::Max) + 1,

    /** Returns:
        - Argument0: a pointer to the framebuffer consisting of pixels in ARGB32 format
        - Argument1: the width of the framebuffer in pixels
        - Argument2: the height of the framebuffer in pixels */
    GetFramebuffer,

    /** Presents the framebuffer. */
    PresentFrame,

    /** Returns:
        - Argument0: next buffered key event, or 0 if none left in the buffer.
            Positive numbers provide SDL_Keycode of pressed keys.
            Negative numbers provide -SDL_Keycode of released keys. */
    ReadKey,

    // TODO(Mark) multithreading and synchronization
    SpawnThread,
    JoinThread,
    ExitThread,
};


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
      emulator { options.memorySize },
      mainThread {},
      renderer { "RISC-V Emulator", options.framebufferWidth, options.framebufferHeight }
{
    auto const programData = readProgramFile(options.programPath);
    riscv::loadElf(programData, emulator);

    try
    {
        framebuffer = emulator.memory.allocate<Renderer::Pixel[]>(
            (size_t) options.framebufferWidth * options.framebufferHeight
        );
    }
    catch (std::bad_alloc const&)
    {
        throw std::runtime_error { "not enough emulator memory to allocate framebuffer" };
    }

    // TODO Register custom syscalls

    renderer.presentEmptyFrame();
}


void App::tick()
{
    emulator.run(mainThread);
    renderer.presentFramebuffer(framebuffer);
}


void App::handleKey(SDL_Keycode keyCode, bool isKeyDown)
{
    if (keyCode == SDLK_UNKNOWN)
        return;

    if (isKeyDown)
        keyEvents.push_back(static_cast<int32_t>(keyCode));
    else
        keyEvents.push_back(-static_cast<int32_t>(keyCode));
}
