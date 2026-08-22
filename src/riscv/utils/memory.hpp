/**
This file provides utilities for unaligned little-endian memory access.
*/
#ifndef RISCV_UTILS_MEMORY_HPP
#define RISCV_UTILS_MEMORY_HPP


#include <bit>
#include <cstdint>
#include <cstring>


namespace riscv
{

static inline uint16_t loadU16(uint8_t const *ptr)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        // memcpy allows unaligned memory access on architectures that don't support it.
        // It should get optimized away for architectures that do.
        uint16_t value;
        std::memcpy(&value, ptr, sizeof(value));
        return value;
    }
    else
    {
        return static_cast<uint16_t>(ptr[0]) | (static_cast<uint16_t>(ptr[1]) << 010);
    }
}

static inline uint32_t loadU32(uint8_t const *ptr)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        uint32_t value;
        std::memcpy(&value, ptr, sizeof(value));
        return value;
    }
    else
    {
        return static_cast<uint32_t>(ptr[0]) | (static_cast<uint32_t>(ptr[1]) << 010)
            | (static_cast<uint32_t>(ptr[2]) << 020) | (static_cast<uint32_t>(ptr[3]) << 030);
    }
}

static inline uint64_t loadU64(uint8_t const *ptr)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        uint64_t value;
        std::memcpy(&value, ptr, sizeof(value));
        return value;
    }
    else
    {
        return static_cast<uint64_t>(ptr[0]) | (static_cast<uint64_t>(ptr[1]) << 010)
            | (static_cast<uint64_t>(ptr[2]) << 020) | (static_cast<uint64_t>(ptr[3]) << 030)
            | (static_cast<uint64_t>(ptr[4]) << 040) | (static_cast<uint64_t>(ptr[5]) << 050)
            | (static_cast<uint64_t>(ptr[6]) << 060) | (static_cast<uint64_t>(ptr[7]) << 070);
    }
}

static inline void storeU16(uint8_t *ptr, uint16_t value)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        std::memcpy(ptr, &value, sizeof(value));
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
        std::memcpy(ptr, &value, sizeof(value));
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
        std::memcpy(ptr, &value, sizeof(value));
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
