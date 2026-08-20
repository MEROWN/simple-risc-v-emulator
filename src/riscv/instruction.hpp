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

    // This contains all RV64G instructions
    enum class Type : uint8_t
    {
        INVALID,

        // RV32I Base Instruction Set
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

        // RV64I Base Instruction Set
        LWU,
        LD,
        SD,
        ADDIW,
        SLLIW,
        SRLIW,
        SRAIW,
        ADDW,
        SUBW,
        SLLW,
        SRLW,
        SRAW,

        // Zifencei Standard Extension
        FENCEI,

        // Zicsr Standard Extension
        CSRRW,
        CSRRS,
        CSRRC,
        CSRRWI,
        CSRRSI,
        CSRRCI,

        // RV32M Standard Extension
        MUL,
        MULH,
        MULHSU,
        MULHU,
        DIV,
        DIVU,
        REM,
        REMU,

        // RV64M Standard Extension
        MULW,
        DIVW,
        DIVUW,
        REMW,
        REMUW,

        // RV32A Standard Extension
        LRW,
        SCW,
        AMOSWAPW,
        AMOADDW,
        AMOXORW,
        AMOANDW,
        AMOORW,
        AMOMINW,
        AMOMAXW,
        AMOMINUW,
        AMOMAXUW,

        // RV64A Standard Extension
        LRD,
        SCD,
        AMOSWAPD,
        AMOADDD,
        AMOXORD,
        AMOANDD,
        AMOORD,
        AMOMIND,
        AMOMAXD,
        AMOMINUD,
        AMOMAXUD,

        // RV32F Standard Extension
        FLW,
        FSW,
        FMADDS,
        FMSUBS,
        FNMADDS,
        FNMSUBS,
        FADDS,
        FSUBS,
        FMULS,
        FDIVS,
        FSQRTS,
        FSGNJS,
        FSGNJNS,
        FSGNJXS,
        FMINS,
        FMAXS,
        FCVTWS,
        FCVTWUS,
        FMVXW,
        FEQS,
        FLTS,
        FLES,
        FCLASSS,
        FCVTSW,
        FCVTSWU,
        FMVWX,

        // RV64F Standard Extension
        FCVTLS,
        FCVTLUS,
        FCVTSL,
        FCVTSLU,

        // RV32D Standard Extension
        FLD,
        FSD,
        FMADDD,
        FMSUBD,
        FNMADDD,
        FNMSUBD,
        FADDD,
        FSUBD,
        FMULD,
        FDIVD,
        FSQRTD,
        FSGNJD,
        FSGNJND,
        FSGNJXD,
        FMIND,
        FMAXD,
        FCVTSD,
        FCVTDS,
        FEQD,
        FLTD,
        FLED,
        FCLASSD,
        FCVTWD,
        FCVTWUD,
        FCVTDW,
        FCVTDWU,

        // RV64D Standard Extension
        FCVTLD,
        FCVTLUD,
        FCVTXD,
        FCVTDL,
        FCVTDLU,
        FMVDX,
    } type;
};

std::vector<Instruction> decodeInstructions(std::span<uint8_t const> program);

} // namespace riscv

#endif
