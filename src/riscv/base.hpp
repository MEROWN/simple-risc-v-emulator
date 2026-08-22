#ifndef RISCV_BASE_HPP
#define RISCV_BASE_HPP


#include <array>
#include <cstdint>


namespace riscv
{


using Pointer = uint64_t;
using Register = Pointer;
using Size = Pointer;

using Offset = int64_t;

/** A register index that refers to general-purpose or floating-point registers. */
using RegisterIndex = uint8_t;

/** Count of general-purpose registers (x0, x1, ..., x31) */
constexpr RegisterIndex registerCount = 32;

/** Count of floating-point registers (f0, f1, ..., f31) */
constexpr RegisterIndex floatRegisterCount = 32;

using FloatRegister = double;


enum class FloatRoundingMode : uint8_t
{
    RNE = 0b000,
    RTZ = 0b001,
    RDN = 0b010,
    RUP = 0b011,
    RMM = 0b100,
    DYN = 0b111,
};


/** Represents integer registers (x0, x1, ..., x31) */
class RegisterFile
{
public:
    Register operator[](RegisterIndex index) const
    {
        return registers[index];
    }

    void set(RegisterIndex index, Register value)
    {
        if (index == 0)
            return;

        registers[index] = value;
    }

private:
    std::array<Register, registerCount> registers {};
};


/** Holds the state of a RISC-V hart (HARdware Thread) */
struct Thread
{
    RegisterFile registers {};

    std::array<FloatRegister, floatRegisterCount> floatRegisters {};

    /** This is semantically a pointer into the original program bytes.
        Jump instructions can change it in arbitrary ways. */
    Register programCounter = 0;


    Pointer getInstructionIndex() const
    {
        return programCounter / 4;
    }

    Pointer getNextProgramCounter() const
    {
        return programCounter + 4;
    }
};


} // namespace riscv


#endif
