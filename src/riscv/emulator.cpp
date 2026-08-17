#include <src/riscv/emulator.hpp>

namespace riscv
{

Emulator::Emulator(Program program, Size memorySize) : program(program)
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
