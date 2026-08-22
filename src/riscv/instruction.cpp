#include <src/riscv/instruction.hpp>

#include <src/utils/bits.hpp>

#include <format>
#include <stdexcept>


using namespace utils;


namespace riscv
{


enum class Opcode : uint8_t
{
    Load = 0b0000011,
    LoadFP = 0b0000111,
    MiscMem = 0b0001111,
    OpImm = 0b0010011,
    AUIPC = 0b0010111,
    OpImm32 = 0b0011011,
    Store = 0b0100011,
    StoreFP = 0b0100111,
    AMO = 0b0101111,
    Op = 0b0110011,
    LUI = 0b0110111,
    Op32 = 0b0111011,
    Madd = 0b1000011,
    Msub = 0b1000111,
    Nmsub = 0b1001011,
    Nmadc = 0b1001111,
    OpFP = 0b1010011,
    OpV = 0b1010111,
    Branch = 0b1100011,
    JALR = 0b1100111,
    JAL = 0b1101111,
    System = 0b1110011,
    OpVE = 0b1110111,
};


class UnknownInstructionException : public std::runtime_error
{
public:
    UnknownInstructionException(uint32_t encoded, std::string const& clazz)
        : std::runtime_error(std::format("Unknown instruction (class {}): {:b} ", clazz, encoded))
    {
    }
};


static bool isEncodingCompressed(uint8_t const *instructionBytes)
{
    uint8_t lowestByte = instructionBytes[0];
    return (lowestByte & 0b11) != 0b11;
}


/** Gets the Operation Code (opcode), which gives a loose classification of the instruction. */
static Opcode getOpcode(uint32_t encoded)
{
    return static_cast<Opcode>(getBits(encoded, 0, 6));
}

/** Gets the Functional 3-bit field, which partially encodes the instruction type. */
static uint8_t getFunct3(uint32_t encoded)
{
    return static_cast<uint8_t>(getBits(encoded, 12, 14));
}

/** Gets the Functional 7-bit field, which partially encodes the instruction type. */
static uint8_t getFunct7(uint32_t encoded)
{
    return static_cast<uint8_t>(getBits(encoded, 25, 31));
}

static RegisterIndex getDestinationRegister(uint32_t encoded)
{
    return static_cast<RegisterIndex>(getBits(encoded, 7, 11));
}

static RegisterIndex getSourceRegister1(uint32_t encoded)
{
    return static_cast<RegisterIndex>(getBits(encoded, 15, 19));
}

static RegisterIndex getSourceRegister2(uint32_t encoded)
{
    return static_cast<RegisterIndex>(getBits(encoded, 20, 24));
}

/** Gets the immediate value for an I-type instruction. */
static int32_t getImmediateI(uint32_t encoded)
{
    return signExtend(getBits(encoded, 20, 31), 11);
}

/** Gets the immediate value for an S-type instruction. */
static int32_t getImmediateS(uint32_t encoded)
{
    return signExtend(getBits(encoded, 7, 11) | (getBits(encoded, 25, 31) << 5), 11);
}

/** Gets the immediate value for a B-type instruction. */
static int32_t getImmediateB(uint32_t encoded)
{
    return signExtend(
        (getBits(encoded, 8, 11) << 1) | (getBits(encoded, 25, 30) << 5)
            | (getBits(encoded, 7, 7) << 11) | (getBits(encoded, 31, 31) << 12),
        12
    );
}

/** Gets the immediate value for an U-type instruction. */
static int32_t getImmediateU(uint32_t encoded)
{
    return static_cast<int32_t>(getBits(encoded, 12, 31) << 12);
}

/** Gets the immediate value for an J-type instruction. */
static int32_t getImmediateJ(uint32_t encoded)
{
    return signExtend(
        (getBits(encoded, 21, 30) << 1) | (getBits(encoded, 20, 20) << 11)
            | (getBits(encoded, 12, 19) << 12) | (getBits(encoded, 31, 31) << 20),
        20
    );
}

/** Decodes the fields for an R-type instruction. */
static void readFieldsR(uint32_t encoded, Instruction& instr, uint8_t& funct3, uint8_t& funct7)
{
    instr.sourceRegister1 = getSourceRegister1(encoded);
    instr.sourceRegister2 = getSourceRegister2(encoded);
    instr.destinationRegister = getDestinationRegister(encoded);
    funct3 = getFunct3(encoded);
    funct7 = getFunct7(encoded);
}

/** Decodes the fields for an I-type instruction. */
static void readFieldsI(uint32_t encoded, Instruction& instr, uint8_t& funct3)
{
    instr.sourceRegister1 = getSourceRegister1(encoded);
    instr.destinationRegister = getDestinationRegister(encoded);
    instr.immediate = getImmediateI(encoded);
    funct3 = getFunct3(encoded);
}

/** Decodes the fields for an S-type instruction. */
static void readFieldsS(uint32_t encoded, Instruction& instr, uint8_t& funct3)
{
    instr.sourceRegister1 = getSourceRegister1(encoded);
    instr.sourceRegister2 = getSourceRegister2(encoded);
    instr.immediate = getImmediateS(encoded);
    funct3 = getFunct3(encoded);
}

/** Decodes the fields for a B-type instruction. */
static void readFieldsB(uint32_t encoded, Instruction& instr, uint8_t& funct3)
{
    instr.sourceRegister1 = getSourceRegister1(encoded);
    instr.sourceRegister2 = getSourceRegister2(encoded);
    instr.immediate = getImmediateB(encoded);
    funct3 = getFunct3(encoded);
}

/** Decodes the fields for an U-type instruction. */
static void readFieldsU(uint32_t encoded, Instruction& instr)
{
    instr.destinationRegister = getDestinationRegister(encoded);
    instr.immediate = getImmediateU(encoded);
}

/** Decodes the fields for a J-type instruction. */
static void readFieldsJ(uint32_t encoded, Instruction& instr)
{
    instr.destinationRegister = getDestinationRegister(encoded);
    instr.immediate = getImmediateJ(encoded);
}


static void decodeBranch(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3;
    readFieldsB(encoded, instr, funct3);

    switch (funct3)
    {
    case 0b000:
        instr.type = Instruction::Type::BEQ;
        break;
    case 0b001:
        instr.type = Instruction::Type::BNE;
        break;
    case 0b100:
        instr.type = Instruction::Type::BLT;
        break;
    case 0b101:
        instr.type = Instruction::Type::BGE;
        break;
    case 0b110:
        instr.type = Instruction::Type::BLTU;
        break;
    case 0b111:
        instr.type = Instruction::Type::BGEU;
        break;

    default:
        throw UnknownInstructionException(encoded, "BRANCH");
    }
}

static void decodeLoad(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3;
    readFieldsI(encoded, instr, funct3);

    switch (funct3)
    {
    case 0b000:
        instr.type = Instruction::Type::LB;
        break;
    case 0b001:
        instr.type = Instruction::Type::LH;
        break;
    case 0b010:
        instr.type = Instruction::Type::LW;
        break;
    case 0b100:
        instr.type = Instruction::Type::LBU;
        break;
    case 0b101:
        instr.type = Instruction::Type::LHU;
        break;

    // RV64I
    case 0b110:
        instr.type = Instruction::Type::LWU;
        break;
    case 0b011:
        instr.type = Instruction::Type::LD;
        break;

    default:
        throw UnknownInstructionException(encoded, "LOAD");
    }
}

static void decodeStore(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3;
    readFieldsS(encoded, instr, funct3);

    switch (funct3)
    {
    case 0b000:
        instr.type = Instruction::Type::SB;
        break;
    case 0b001:
        instr.type = Instruction::Type::SH;
        break;
    case 0b010:
        instr.type = Instruction::Type::SW;
        break;

    // RV64I
    case 0b011:
        instr.type = Instruction::Type::SD;
        break;

    default:
        throw UnknownInstructionException(encoded, "STORE");
    }
}

static void decodeOpImm(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3;
    readFieldsI(encoded, instr, funct3);

    switch (funct3)
    {
    case 0b000:
        instr.type = Instruction::Type::ADDI;
        break;
    case 0b001:
        instr.type = Instruction::Type::SLLI;
        break;
    case 0b010:
        instr.type = Instruction::Type::SLTI;
        break;
    case 0b011:
        instr.type = Instruction::Type::SLTIU;
        break;
    case 0b100:
        instr.type = Instruction::Type::XORI;
        break;

    case 0b101:
        if (hasBit(instr.immediate, 10))
        {
            clearBit(instr.immediate, 10);
            instr.type = Instruction::Type::SRAI;
        }
        else
        {
            instr.type = Instruction::Type::SRLI;
        }
        break;

    case 0b110:
        instr.type = Instruction::Type::ORI;
        break;
    case 0b111:
        instr.type = Instruction::Type::ANDI;
        break;
    }
}

static void decodeOpImm32(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3;
    readFieldsI(encoded, instr, funct3);

    switch (funct3)
    {
    case 0b000:
        instr.type = Instruction::Type::ADDIW;
        break;

    case 0b001:
        instr.type = Instruction::Type::SLLIW;
        break;

    case 0b101:
        if (hasBit(instr.immediate, 10))
        {
            clearBit(instr.immediate, 10);
            instr.type = Instruction::Type::SRAIW;
        }
        else
        {
            instr.type = Instruction::Type::SRLIW;
        }
        break;

    default:
        throw UnknownInstructionException(encoded, "OP-IMM32");
    }
}

static void decodeOp(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3, funct7;
    readFieldsR(encoded, instr, funct3, funct7);

    switch (funct7)
    {
    case 0b0000000:
        switch (funct3)
        {
        case 0b000:
            if (hasBit(funct7, 5))
            {
                clearBit(funct7, 5);
                instr.type = Instruction::Type::SUB;
            }
            else
            {
                instr.type = Instruction::Type::ADD;
            }
            break;

        case 0b001:
            instr.type = Instruction::Type::SLL;
            break;

        case 0b010:
            instr.type = Instruction::Type::SLT;
            break;

        case 0b011:
            instr.type = Instruction::Type::SLTU;
            break;

        case 0b100:
            instr.type = Instruction::Type::XOR;
            break;

        case 0b101:
            if (hasBit(funct7, 5))
            {
                clearBit(funct7, 5);
                instr.type = Instruction::Type::SRA;
            }
            else
            {
                instr.type = Instruction::Type::SRL;
            }
            break;

        case 0b110:
            instr.type = Instruction::Type::OR;
            break;

        case 0b111:
            instr.type = Instruction::Type::AND;
            break;
        }

        break;

    // RV32M
    case 0b0000001:
        switch (funct3)
        {
        case 0b000:
            instr.type = Instruction::Type::MUL;
            break;

        case 0b001:
            instr.type = Instruction::Type::MULH;
            break;

        case 0b010:
            instr.type = Instruction::Type::MULHSU;
            break;

        case 0b011:
            instr.type = Instruction::Type::MULHU;
            break;

        case 0b100:
            instr.type = Instruction::Type::DIV;
            break;

        case 0b101:
            instr.type = Instruction::Type::DIVU;
            break;

        case 0b110:
            instr.type = Instruction::Type::REM;
            break;

        case 0b111:
            instr.type = Instruction::Type::REMU;
            break;
        }
        break;

    default:
        throw UnknownInstructionException(encoded, "OP");
    }
}

static void decodeOp32(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3, funct7;
    readFieldsR(encoded, instr, funct3, funct7);

    switch (funct7)
    {
    case 0b0000000:
        switch (funct3)
        {
        case 0b000:
            if (hasBit(funct7, 5))
            {
                clearBit(funct7, 5);
                instr.type = Instruction::Type::SUBW;
            }
            else
            {
                instr.type = Instruction::Type::ADDW;
            }
            break;

        case 0b001:
            instr.type = Instruction::Type::SLLW;
            break;

        case 0b101:
            if (hasBit(funct7, 5))
            {
                clearBit(funct7, 5);
                instr.type = Instruction::Type::SRAW;
            }
            else
            {
                instr.type = Instruction::Type::SRLW;
            }
            break;

        default:
            throw UnknownInstructionException(encoded, "OP-32");
        }

        break;

    // RV64M
    case 0b0000001:
        switch (funct3)
        {
        case 0b000:
            instr.type = Instruction::Type::MULW;
            break;

        case 0b100:
            instr.type = Instruction::Type::DIVW;
            break;

        case 0b101:
            instr.type = Instruction::Type::DIVUW;
            break;

        case 0b110:
            instr.type = Instruction::Type::REMW;
            break;

        case 0b111:
            instr.type = Instruction::Type::REMUW;
            break;

        default:
            throw UnknownInstructionException(encoded, "OP-32");
        }

        break;

    default:
        throw UnknownInstructionException(encoded, "OP-32");
    }
}

static void decodeMiscMem(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3;
    readFieldsI(encoded, instr, funct3);

    switch (funct3)
    {
    case 0b000:
        instr.immediate &= 0b1111'1111'1111;

        if (instr.immediate == 0b1000'0011'0011)
            instr.type = Instruction::Type::FENCETSO;
        else if (instr.immediate == 0b0000'0001'0000)
            instr.type = Instruction::Type::PAUSE;
        else
            instr.type = Instruction::Type::FENCE;

        break;

    // Zifencei
    case 0b001:
        instr.type = Instruction::Type::FENCEI;
        break;

    default:
        throw UnknownInstructionException(encoded, "MISC-MEM");
    }
}

static void decodeSystem(uint32_t encoded, Instruction& instr)
{
    uint8_t funct3;
    readFieldsI(encoded, instr, funct3);

    auto immediate = instr.immediate;
    instr.immediate = 0;

    switch (funct3)
    {
    case 0b000:
        if (immediate == 0)
            instr.type = Instruction::Type::ECALL;
        else if (immediate == 1)
            instr.type = Instruction::Type::EBREAK;
        else
            throw UnknownInstructionException(encoded, "SYSTEM");
        break;

    // Zicsr
    case 0b001:
        instr.type = Instruction::Type::CSRRW;
        break;
    case 0b010:
        instr.type = Instruction::Type::CSRRS;
        break;
    case 0b011:
        instr.type = Instruction::Type::CSRRC;
        break;
    case 0b101:
        instr.type = Instruction::Type::CSRRWI;
        break;
    case 0b110:
        instr.type = Instruction::Type::CSRRSI;
        break;
    case 0b111:
        instr.type = Instruction::Type::CSRRCI;
        break;

    default:
        throw UnknownInstructionException(encoded, "SYSTEM");
    }
}


static void decodeInstruction(Instruction& instr, uint32_t encoded)
{
    switch (getOpcode(encoded))
    {
    case Opcode::LUI:
        readFieldsU(encoded, instr);
        instr.type = Instruction::Type::LUI;
        break;

    case Opcode::AUIPC:
        readFieldsU(encoded, instr);
        instr.type = Instruction::Type::AUIPC;
        break;

    case Opcode::JAL:
        readFieldsJ(encoded, instr);
        instr.type = Instruction::Type::JAL;
        break;

    case Opcode::JALR:
    {
        uint8_t funct3;
        readFieldsI(encoded, instr, funct3);
        instr.type = Instruction::Type::JALR;
    }
    break;

    case Opcode::Branch:
        decodeBranch(encoded, instr);
        break;

    case Opcode::Load:
        decodeLoad(encoded, instr);
        break;

    case Opcode::Store:
        decodeStore(encoded, instr);
        break;

    case Opcode::OpImm:
        decodeOpImm(encoded, instr);
        break;

    case Opcode::OpImm32:
        decodeOpImm32(encoded, instr);
        break;

    case Opcode::Op:
        decodeOp(encoded, instr);
        break;

    case Opcode::Op32:
        decodeOp32(encoded, instr);
        break;

    case Opcode::MiscMem:
        decodeMiscMem(encoded, instr);
        break;

    case Opcode::System:
        decodeSystem(encoded, instr);
        break;

    default:
        throw UnknownInstructionException(encoded, "unknown");
    }
}


std::vector<Instruction> decodeInstructions(std::span<uint8_t const> program)
{
    if (program.size() % 4 != 0)
    {
        throw std::runtime_error(
            "Program size not divisible by 4: " + std::to_string(program.size())
        );
    }

    std::vector<Instruction> instructions {};

    for (Pointer programOffset = 0; programOffset < program.size(); programOffset += 4)
    {
        auto instructionBytes = program.data() + programOffset;

        if (isEncodingCompressed(instructionBytes))
            throw std::runtime_error("Compressed instructions are not supported");

        uint32_t encoded = loadU32(instructionBytes);
        Instruction instruction {};
        decodeInstruction(instruction, encoded);

        instructions.push_back(instruction);
    }

    return instructions;
}

} // namespace riscv
