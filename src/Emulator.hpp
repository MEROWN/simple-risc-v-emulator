#ifndef EMULATOR_HPP
#define EMULATOR_HPP


#include <cstdint>
#include <span>

namespace riscv
{

using Byte = std::uint8_t;
using Halfword = std::uint16_t;
using Word = std::uint32_t;
using Doubleword = std::uint64_t;
using Pointer = std::uint64_t;

using Instruction = Word;

struct EmulatorInfo
{
    Pointer memorySize = UINT64_C(16) * 1024 * 1024; // 16 MB
    std::span<Instruction const> instructions;
};

class Emulator
{
    Emulator(EmulatorInfo const& info);
    ~Emulator();

    void run();

    // TODO Add a way to read the DMA framebuffer from the memory
    // TODO Add a way to read the main memory
    // TODO Add a way to run on multiple cores
};


}; // namespace riscv


#endif
