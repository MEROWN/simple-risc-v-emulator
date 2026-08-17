#ifndef EMULATOR_HPP
#define EMULATOR_HPP


#include <cstdint>
#include <span>

namespace riscv
{

using Byte = uint8_t;
using Halfword = uint16_t;
using Word = uint32_t;
using Doubleword = uint64_t;
using Pointer = uint64_t;

using Instruction = Word;

struct EmulatorInfo
{
    Pointer memorySize = (Pointer) 16 * 1024 * 1024; // 16 MB
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
