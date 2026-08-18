#ifndef RISCV_HPP
#define RISCV_HPP


#include <cstdint>


namespace riscv
{

using Byte = uint8_t;
using Halfword = uint16_t;
using Word = uint32_t;
using Doubleword = uint64_t;

using Register = uint64_t;

using Pointer = uint64_t;
using Size = Pointer;

constexpr auto registerCount = 32;

} // namespace riscv


#endif
