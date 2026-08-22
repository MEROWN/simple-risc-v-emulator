#ifndef RISCV_INSTRUCTION_HPP
#define RISCV_INSTRUCTION_HPP


#include <src/riscv/base.hpp>

#include <span>
#include <vector>


namespace riscv
{


struct Instruction
{
    struct FloatOperationParams
    {
        FloatRoundingMode roundingMode;
        RegisterIndex sourceRegister3;
    };

    union
    {
        int32_t immediate = 0;
        FloatOperationParams floatOp;
    };

    RegisterIndex destinationRegister = 0;
    RegisterIndex sourceRegister1 = 0;
    RegisterIndex sourceRegister2 = 0;

    // This contains all RV64G instructions
    enum class Type : uint8_t
    {
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
        FEQS,
        FLTS,
        FLES,
        FCLASSS,
        FCVTWS,
        FCVTSW,
        FCVTWUS,
        FCVTSWU,
        FMVXW,
        FMVWX,

        // RV64F Standard Extension
        FCVTLS,
        FCVTSL,
        FCVTLUS,
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
        FEQD,
        FLTD,
        FLED,
        FCLASSD,
        FCVTSD,
        FCVTDS,
        FCVTWD,
        FCVTDW,
        FCVTWUD,
        FCVTDWU,

        // RV64D Standard Extension
        FCVTLD,
        FCVTDL,
        FCVTLUD,
        FCVTDLU,
        FMVXD,
        FMVDX,
    } type = (Type) 0;
};

std::vector<Instruction> decodeInstructions(std::span<uint8_t const> program);

} // namespace riscv

#endif
