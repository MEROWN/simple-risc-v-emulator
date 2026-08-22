#ifndef EMULATOR_HPP
#define EMULATOR_HPP


#include <src/riscv/instruction.hpp>
#include <src/riscv/riscv.hpp>

#include <span>
#include <vector>


namespace riscv
{


/** Holds the state of a RISC-V hart (HARdware Thread) */
class Thread
{
public:
    Register getRegister(uint8_t registerIndex) const
    {
        return registers[registerIndex];
    }

    void setRegister(uint8_t registerIndex, Register value)
    {
        if (registerIndex != 0)
            registers[registerIndex] = value;
    }

    Pointer getInstructionIndex() const
    {
        return programCounter / 4;
    }

private:
    std::array<Register, registerCount> registers {};

    /** This is semantically a pointer into the original program bytes.
        Every instruction increments this by 4. */
    Register programCounter = 0;

    Pointer getNextProgramCounter() const
    {
        return programCounter + 4;
    }

    friend class Emulator;
};


class Emulator
{
public:
    Emulator();
    ~Emulator();

    void run(Thread& thread);

    /** The returned span remains valid as long as the memory is not resized. */
    std::span<uint8_t> getMemory()
    {
        return memory;
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

    /** This validates and decodes the provided program and loads it into the emulator. */
    void loadInstructions(std::span<uint8_t const> program)
    {
        setInstructions(decodeInstructions(program));
    }


private:
    std::vector<uint8_t> memory {};
    std::vector<Instruction> instructions {};
};


} // namespace riscv


#endif
