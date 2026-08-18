#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP


#include <src/riscv/riscv.hpp>

#include <span>
#include <vector>


namespace riscv
{

struct Instruction
{
    uint32_t immediate;

    uint8_t destinationRegister;
    uint8_t sourceRegister1;
    uint8_t sourceRegister2;

    enum class Type : uint8_t
    {
        LUI,
        AUIPC,
        JAL,
        JALR,
        BEQ,
        BNE,
        BLT,
        BGE,
        BLTU,
        BGEU,
        LB,
        LH,
        LW,
        LBU,
        LHU,
        SB,
        SH,
        SW,
        ADDI,
        SLTI,
        SLTIU,
        XORI,
        ORI,
        ANDI,
        SLLI,
        SRLI,
        SRAI,
        ADD,
        SUB,
        SLL,
        SLT,
        SLTU,
        XOR,
        SRL,
        SRA,
        OR,
        AND,
        FENCE,
        FENCETSO,
        PAUSE,
        ECALL,
        EBREAK,
    } type;
};

std::vector<Instruction> decodeInstructions(std::span<Byte const> program);

} // namespace riscv

#endif
