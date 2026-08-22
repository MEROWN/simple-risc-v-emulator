#ifndef RISCV_EMULATOR_HPP
#define RISCV_EMULATOR_HPP


#include <src/riscv/instruction.hpp>

#include <span>
#include <vector>


namespace riscv
{


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
