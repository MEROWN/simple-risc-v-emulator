#include <src/riscv/emulator.hpp>


namespace riscv
{


Emulator::Emulator()
{
}

Emulator::~Emulator()
{
}

void Emulator::run()
{
    // TODO(Danil) instruction execution loop

    // Remove this code
    memory[0] = 0x00; // This sets the top-left pixel to orange
    memory[1] = 0x77;
    memory[2] = 0xff;
    memory[3] = 0xff;
}


} // namespace riscv
