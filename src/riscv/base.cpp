#include "base.hpp"

#include <src/riscv/utils/bits.hpp>


namespace riscv
{

void Thread::setFloatExceptionFlag(FloatExceptionFlags flag, bool value)
{
    if (value)
        setBit(floatExceptionFlags, static_cast<uint8_t>(flag));
    else
        clearBit(floatExceptionFlags, static_cast<uint8_t>(flag));
}

bool Thread::getFloatExceptionFlag(FloatExceptionFlags flag) const
{
    return hasBit(floatExceptionFlags, static_cast<uint8_t>(flag));
}

Register Thread::getFloatControlStatusRegister() const
{
    return (static_cast<Register>(dynamicFloatRoundingMode) << 5)
        | static_cast<Register>(floatExceptionFlags);
}

void Thread::setFloatControlStatusRegister(Register value)
{
    dynamicFloatRoundingMode = static_cast<FloatRoundingMode>(getBits(value, 5, 7));
    floatExceptionFlags = static_cast<uint8_t>(getBits(value, 0, 4));
}

} // namespace riscv
