#ifndef EMULATOR_HPP
#define EMULATOR_HPP


#include <src/riscv/riscv.hpp>

#include <span>
#include <vector>


namespace emulator
{

using namespace riscv;

/** ARGB32LE: ARGB 32-bit little-endian pixel format.
    The memory layout is as follows: B, G, R, A.
    A is in the highest byte, B is in the lowest byte.
    This matches the endianness of RISC-V (which little-endian). */
using Pixel = Word;

struct EmulatorInfo
{
    Pointer freeMemorySize;

    Word framebufferWidth;
    Word framebufferHeight;

    std::span<Instruction const> instructions;
};

class Emulator
{
public:
    Emulator(EmulatorInfo const& info);
    ~Emulator();

    void run();

    std::vector<Byte> memory;

private:
};


}; // namespace emulator


#endif
