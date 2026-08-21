#ifndef UTILS_HPP
#define UTILS_HPP


#include <cstdint>


namespace utils
{


template <typename T>
static constexpr bool hasBit(T value, uint8_t bitIndex)
{
    return (value & (static_cast<T>(1) << bitIndex)) != 0;
}

template <typename T>
static constexpr void setBit(T& value, uint8_t bitIndex)
{
    value |= (static_cast<T>(1) << bitIndex);
}

template <typename T>
static constexpr void clearBit(T& value, uint8_t bitIndex)
{
    value &= ~(static_cast<T>(1) << bitIndex);
}

/** Returns a bitmask with the specified number of bits set to 1.
    Example: oneBits<uint32_t>(3) = 0b111 */
template <typename T>
static constexpr T oneBits(uint8_t count)
{
    return (static_cast<T>(1) << count) - 1;
}

/** Gets the value of a bitfield located in the given bits. */
template <typename T>
static constexpr T getBits(T value, uint8_t firstBit, uint8_t lastBit)
{
    return (value >> firstBit) & oneBits<T>(lastBit - firstBit + 1);
}

/** Sign-extends a value using the specified sign bit.
    Example: signExtend<uint8_t>(0b00010000, 4) = 0b11110000 */
template <typename T>
static constexpr T signExtend(T value, uint8_t signBit)
{
    T signBitMask = value & (1 << signBit);
    T extendedSignBitMask = -signBitMask;
    return value | extendedSignBitMask;
}


} // namespace utils


#endif
