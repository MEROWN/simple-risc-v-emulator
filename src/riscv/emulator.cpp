#include <src/riscv/emulator.hpp>

namespace riscv
{

Emulator::Emulator(std::span<riscv::Instruction const> program, Size memorySize)
    : program { program }
{
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

} // namespace riscv
