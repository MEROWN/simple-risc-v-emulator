#include <src/riscv/emulator.hpp>

#include <vector>


namespace riscv
{

struct Emulator::Instruction
{
    // TODO This must contain *decoded* instruction data.
    riscv::Word removeThis;
};


std::vector<Emulator::Instruction> Emulator::decodeInstructions(std::span<Byte const> program)
{
    std::vector<Instruction> instructions {};
    // TODO Decode instructions
    return instructions;
}


Emulator::Emulator(std::span<Byte const> program, Size memorySize)
    : instructions { decodeInstructions(program) }, memory { std::vector<Byte>(memorySize) }
{
    // TODO
}

Emulator::~Emulator()
{
    // TODO
}

void Emulator::run()
{
    // TODO
}

} // namespace riscv
