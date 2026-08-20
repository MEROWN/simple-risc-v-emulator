#ifndef RISCV_HPP
#define RISCV_HPP


#include <bit>
#include <cstdint>


namespace riscv
{


// Use fixed-sized integer types everywhere, except for:

using Pointer = uint64_t;
using Register = Pointer;
using Size = Pointer;

using Offset = int64_t;

/** Contains values from 0 to registerCount-1. */
using RegisterIndex = uint8_t;

/** Count of general-purpose registers (x0, x1, ..., x31) */
constexpr RegisterIndex registerCount = 32;


static inline uint16_t loadU16(uint8_t const *ptr)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return *reinterpret_cast<uint16_t const *>(ptr);
    }
    else
    {
        return static_cast<uint16_t>(ptr[0]) | (static_cast<uint16_t>(ptr[1]) << 010);
    }
}

static inline uint32_t loadU32(uint8_t const *ptr)
{
    if constexpr (std::endian::native == std::endian::little)
        return *reinterpret_cast<uint32_t const *>(ptr);
    else
        return static_cast<uint32_t>(ptr[0]) | (static_cast<uint32_t>(ptr[1]) << 010)
            | (static_cast<uint32_t>(ptr[2]) << 020) | (static_cast<uint32_t>(ptr[3]) << 030);
}

static inline uint64_t loadU64(uint8_t const *ptr)
{
    if constexpr (std::endian::native == std::endian::little)
        return *reinterpret_cast<uint64_t const *>(ptr);
    else
        return static_cast<uint64_t>(ptr[0]) | (static_cast<uint64_t>(ptr[1]) << 010)
            | (static_cast<uint64_t>(ptr[2]) << 020) | (static_cast<uint64_t>(ptr[3]) << 030)
            | (static_cast<uint64_t>(ptr[4]) << 040) | (static_cast<uint64_t>(ptr[5]) << 050)
            | (static_cast<uint64_t>(ptr[6]) << 060) | (static_cast<uint64_t>(ptr[7]) << 070);
}

static inline void storeU16(uint8_t *ptr, uint16_t value)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        *reinterpret_cast<uint16_t *>(ptr) = value;
    }
    else
    {
        ptr[0] = static_cast<uint8_t>(value);
        ptr[1] = static_cast<uint8_t>(value >> 010);
    }
}

static inline void storeU32(uint8_t *ptr, uint32_t value)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        *reinterpret_cast<uint32_t *>(ptr) = value;
    }
    else
    {
        ptr[0] = static_cast<uint8_t>(value);
        ptr[1] = static_cast<uint8_t>(value >> 010);
        ptr[2] = static_cast<uint8_t>(value >> 020);
        ptr[3] = static_cast<uint8_t>(value >> 030);
    }
}

static inline void storeU64(uint8_t *ptr, uint64_t value)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        *reinterpret_cast<uint64_t *>(ptr) = value;
    }
    else
    {
        ptr[0] = static_cast<uint8_t>(value);
        ptr[1] = static_cast<uint8_t>(value >> 010);
        ptr[2] = static_cast<uint8_t>(value >> 020);
        ptr[3] = static_cast<uint8_t>(value >> 030);
        ptr[4] = static_cast<uint8_t>(value >> 040);
        ptr[5] = static_cast<uint8_t>(value >> 050);
        ptr[6] = static_cast<uint8_t>(value >> 060);
        ptr[7] = static_cast<uint8_t>(value >> 070);
    }
}


} // namespace riscv


#endif
