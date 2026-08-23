/**
This file provides utilities for bit manipulation.
*/
#ifndef RISCV_UTILS_BITS_HPP
#define RISCV_UTILS_BITS_HPP


#include <cstdint>
#include <type_traits>


namespace riscv
{


template <typename T>
static constexpr bool hasBit(T value, uint8_t bitIndex)
{
    return (value & (T { 1 } << bitIndex)) != 0;
}

template <typename T>
static constexpr void setBit(T& value, uint8_t bitIndex)
{
    value |= (T { 1 } << bitIndex);
}

template <typename T>
static constexpr void clearBit(T& value, uint8_t bitIndex)
{
    value &= ~(T { 1 } << bitIndex);
}

/** Returns a bitmask with the specified number of bits set to 1.
    Example: oneBits<uint32_t>(3) = 0b111 */
template <typename T>
static constexpr T oneBits(uint8_t count)
{
    return (T { 1 } << count) - T { 1 };
}

/** Gets the value of a bitfield located in the given bits. */
template <typename T>
static constexpr T getBits(T value, uint8_t firstBit, uint8_t lastBit)
{
    return (value >> firstBit) & oneBits<T>(lastBit - firstBit + 1);
}

/** Sign-extends a value using the specified sign bit.
    Example: signExtend<uint8_t>(0b0001'0000, 4) = 0b1111'0000 */
template <typename T>
static constexpr std::make_signed_t<T> signExtend(T value, uint8_t signBit)
{
    using S = std::make_signed_t<T>;

    unsigned int beforeWidth = signBit + 1;
    unsigned int afterWidth = sizeof(T) * 8;
    unsigned int shiftAmount = afterWidth - beforeWidth;

    // This code needs static_cast and not bit_cast because S can be promoted to the possibly
    // larger int type, which may cause the shifts to not work correctly (i.e. sign-extend).
    return static_cast<S>(static_cast<S>(value << shiftAmount) >> shiftAmount);
}


} // namespace riscv


#endif
