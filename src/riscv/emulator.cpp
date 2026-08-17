#include <src/riscv/emulator.hpp>

namespace emulator
{

/** System information file contains:
    - Framebuffer height: Word
    - Framebuffer width: Word
*/
constexpr Pointer SystemInfoFileSize = 2 * sizeof(Word);

Emulator::Emulator(EmulatorInfo const& info)
{
    Pointer memorySize = info.freeMemorySize
        + sizeof(Pixel) * info.framebufferWidth * info.framebufferHeight + SystemInfoFileSize;

    memory = std::vector<Byte>(memorySize);
}

Emulator::~Emulator()
{
    // TODO
}

void Emulator::run()
{
    // TODO
}

} // namespace emulator
