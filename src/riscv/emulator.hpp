#ifndef EMULATOR_HPP
#define EMULATOR_HPP


#include <src/riscv/instruction.hpp>
#include <src/riscv/riscv.hpp>

#include <span>
#include <vector>


namespace riscv
{


class Emulator
{
public:
    Emulator();
    ~Emulator();

    void run();

    std::span<Byte> getMemory()
    {
        return memory;
    }

    std::array<Register, registerCount> const& getRegisters()
    {
        return registers;
    }

    /** This may invalidate all previously acquired memory spans. */
    void resizeMemory(Size newSize)
    {
        memory.resize(newSize);
    }

    /** This moves the provided instructions into the emulator. */
    void setInstructions(std::vector<Instruction> newInstructions)
    {
        instructions = std::move(newInstructions);
    }

    /** This validates & decodes the provided program and loads it into the emulator. */
    void loadInstructions(std::span<Byte const> program)
    {
        setInstructions(decodeInstructions(program));
    }


private:
    std::array<Register, registerCount> registers {};
    std::vector<Instruction> instructions {};
    std::vector<Byte> memory {};
    Register programCounter = 0;
    void writeRegister(uint8_t registerIndex, Register value)
    {
        if (registerIndex == 0)
        {
            return;
        }
        registers[registerIndex] = value;
    }
};


} // namespace riscv


#endif
