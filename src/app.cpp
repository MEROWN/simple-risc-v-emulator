#include <src/app.hpp>

#include <src/options.hpp>

#include <fstream>


enum class CustomSyscall : uint64_t
{
    /** Returns:
        - Argument0: a random 64-bit integer number */
    Random = static_cast<uint64_t>(riscv::BuiltinSyscall::Max) + 1,

    /** Returns:
        - Argument0: the pointer to the framebuffer */
    GetFramebuffer,

    /** Returns:
        - Argument0: the width of the framebuffer in pixels
        - Argument1: the height of the framebuffer in pixels */
    GetFramebufferSize,

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
    // TODO(Mark) implement reading ELF binaries and mapping data segments into the memory.
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
    // TODO Register custom syscalls

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
    if (keyCode == SDLK_UNKNOWN)
        return;

    if (isKeyDown)
        keyEvents.push_back(static_cast<int32_t>(keyCode));
    else
        keyEvents.push_back(-static_cast<int32_t>(keyCode));
}
