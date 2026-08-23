#ifndef RISCV_BASE_HPP
#define RISCV_BASE_HPP


#include <array>
#include <chrono>
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

/** Software ABI register aliases. Contains only the registers usable for the emulator. */
enum ABIRegisterIndex : RegisterIndex
{
    ReturnAddress = 1,
    StackPointer = 2,
    GlobalPointer = 3,
    ThreadPointer = 4,

    Argument0 = 10,
    Argument1 = 11,
    Argument2 = 12,
    Argument3 = 13,
    Argument4 = 14,
    Argument5 = 15,
    Argument6 = 16,
    Argument7 = 17,
};

/** Software ABI float register aliases. Contains only the registers usable for the emulator. */
enum class ABIFloatRegisterIndex : RegisterIndex
{
    Argument0 = 10,
    Argument1 = 11,
    Argument2 = 12,
    Argument3 = 13,
    Argument4 = 14,
    Argument5 = 15,
    Argument6 = 16,
    Argument7 = 17,
};


enum class ControlStatusRegister : uint16_t
{
    FloatExceptionFlags = 0x001,
    FloatRoundingMode = 0x002,
    FloatControlStatusRegister = 0x003,

    Cycle = 0xC00,
    Time = 0xC01,
    Instret = 0xC02,
};

enum class FloatRoundingMode : uint8_t
{
    RoundToNearestEven = 0b000,
    RoundToZero = 0b001,
    RoundDown = 0b010,
    RoundUp = 0b011,
    RoundToNearestMax = 0b100,
    Dynamic = 0b111,
};

enum class FloatExceptionFlags : uint8_t
{
    Inexact = 0,
    Underflow = 1,
    Overflow = 2,
    DivideByZero = 3,
    InvalidOperation = 4,
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
class Thread
{
public:
    RegisterFile registers {};

    std::array<FloatRegister, floatRegisterCount> floatRegisters {};

    /** This is semantically a pointer into the original program bytes.
            Jump instructions can change it in arbitrary ways. */
    Register programCounter = 0;

    FloatRoundingMode dynamicFloatRoundingMode {};

    uint8_t floatExceptionFlags = 0;

    void setFloatExceptionFlag(FloatExceptionFlags flag, bool value);
    bool getFloatExceptionFlag(FloatExceptionFlags flag) const;
    Register getFloatControlStatusRegister() const;
    void setFloatControlStatusRegister(Register value);

    /** This emulates the `cycle` and `instret` Control Status Registers.
        This should be incremented by 1 after each instruction is executed. */
    uint64_t cycleCounter = 0;

    /** This emulates the `time` Control Status Register.
        Returns the elapsed time in nanoseconds. */
    uint64_t getTime() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - startTime)
            .count();
    }

    Pointer getInstructionIndex() const
    {
        return programCounter / 4;
    }

    Pointer getNextProgramCounter() const
    {
        return programCounter + 4;
    }

private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point startTime = Clock::now();
};


} // namespace riscv


#endif
