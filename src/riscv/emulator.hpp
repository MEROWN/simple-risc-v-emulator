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
    // TODO Add a way to initialize CSR (constrol & status registers)
    Emulator(std::span<Byte const> program, Size memorySize);
    ~Emulator();

    void run();

    inline std::span<Byte> getMemory()
    {
        return memory;
    }

private:
    struct Instruction;

    std::span<Instruction const> instructions;
    std::vector<Byte> memory;

    static std::vector<Instruction> decodeInstructions(std::span<Byte const> program);
};


}; // namespace riscv


#endif
