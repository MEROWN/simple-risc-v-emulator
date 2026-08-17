#ifndef EMULATOR_HPP
#define EMULATOR_HPP


#include <src/riscv/riscv.hpp>

#include <span>
#include <vector>


namespace riscv
{

class Emulator
{
public:
    // TODO Add a way to initialize registers
    Emulator(Program program, Size memorySize);
    ~Emulator();

    void run();

    inline std::span<Byte> getMemory()
    {
        return memory;
    }

private:
    Program program;
    std::vector<Byte> memory;
};


}; // namespace riscv


#endif
