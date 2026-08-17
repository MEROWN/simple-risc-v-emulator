#ifndef RISCV_HPP
#define RISCV_HPP


#include <cstdint>
#include <span>


namespace riscv
{

using Byte = uint8_t;
using Halfword = uint16_t;
using Word = uint32_t;
using Doubleword = uint64_t;

using Pointer = uint64_t;
using Size = Pointer;

using Instruction = Word;
using Program = std::span<Instruction const>;

} // namespace riscv


#endif
