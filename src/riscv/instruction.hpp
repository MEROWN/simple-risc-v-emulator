#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP


#include <src/riscv/riscv.hpp>

#include <span>
#include <vector>


namespace riscv
{

struct Instruction
{
    enum class Type
    {
        Add,
        Sub,
    } type;
};

std::vector<Instruction> decodeInstructions(std::span<Byte const> program);

} // namespace riscv

#endif
