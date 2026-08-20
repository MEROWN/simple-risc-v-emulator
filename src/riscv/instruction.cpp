#include <src/riscv/instruction.hpp>

#include <stdexcept>


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

template <typename T>
static constexpr bool hasBit(T value, uint8_t bitIndex)
{
    return (value & (static_cast<T>(1) << bitIndex)) != 0;
}

template <typename T>
static constexpr void setBit(T& value, uint8_t bitIndex)
{
    value |= (static_cast<T>(1) << bitIndex);
}

template <typename T>
static constexpr void clearBit(T& value, uint8_t bitIndex)
{
    value &= ~(static_cast<T>(1) << bitIndex);
}

/** Returns a bitmask with the specified number of bits set to 1.
    Example: oneBits<uint32_t>(3) returns 0b111 */
template <typename T>
static constexpr T oneBits(uint8_t count)
{
    return (static_cast<T>(1) << count) - 1;
}

/** Gets the value of a bitfield located in the given bits. */
template <typename T>
static constexpr T getBits(T value, uint8_t firstBit, uint8_t lastBit)
{
    return (value >> firstBit) & oneBits<T>(lastBit - firstBit + 1);
}

template <typename T>
static constexpr T signExtend(T value, uint8_t signBit)
{
    T signBitMask = value & (1 << signBit);
    T extendedSignBitMask = -signBitMask;
    return value | extendedSignBitMask;
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
static uint32_t getImmediateI(uint32_t encoded)
{
    return signExtend(getBits(encoded, 20, 31), 11);
}

/** Gets the immediate value for an S-type instruction. */
static uint32_t getImmediateS(uint32_t encoded)
{
    return signExtend(getBits(encoded, 7, 11) | (getBits(encoded, 25, 31) << 5), 11);
}

/** Gets the immediate value for a B-type instruction. */
static uint32_t getImmediateB(uint32_t encoded)
{
    return signExtend(
        (getBits(encoded, 8, 11) << 1) | (getBits(encoded, 25, 30) << 5)
            | (getBits(encoded, 7, 7) << 11) | (getBits(encoded, 31, 31) << 12),
        12
    );
}

/** Gets the immediate value for an U-type instruction. */
static uint32_t getImmediateU(uint32_t encoded)
{
    return getBits(encoded, 12, 31) << 12;
}

/** Gets the immediate value for an J-type instruction. */
static uint32_t getImmediateJ(uint32_t encoded)
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


static void decodeBranch(Instruction& instr, uint8_t funct3)
{
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
        throw std::runtime_error("Illegal BRANCH instruction funct3: " + std::to_string(funct3));
    }
}

static void decodeLoad(Instruction& instr, uint8_t funct3)
{
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

    default:
        throw std::runtime_error("Illegal LOAD instruction funct3: " + std::to_string(funct3));
    }
}

static void decodeStore(Instruction& instr, uint8_t funct3)
{
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

    default:
        throw std::runtime_error("Illegal STORE instruction funct3: " + std::to_string(funct3));
    }
}

static void decodeOpImm(Instruction& instr, uint8_t funct3)
{
    switch (funct3)
    {
    case 0b000:
        instr.type = Instruction::Type::ADDI;
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
    case 0b110:
        instr.type = Instruction::Type::ORI;
        break;
    case 0b111:
        instr.type = Instruction::Type::ANDI;
        break;
    case 0b001:
        instr.type = Instruction::Type::SLLI;
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

    default:
        throw std::runtime_error("Illegal OP-IMM instruction funct3: " + std::to_string(funct3));
    }
}

static void decodeOp(Instruction& instr, uint8_t funct3, uint8_t funct7)
{
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

    default:
        throw std::runtime_error("Illegal OP instruction funct3: " + std::to_string(funct3));
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
    {
        uint8_t funct3;
        readFieldsB(encoded, instr, funct3);
        decodeBranch(instr, funct3);
    }
    break;

    case Opcode::Load:
    {
        uint8_t funct3;
        readFieldsI(encoded, instr, funct3);
        decodeLoad(instr, funct3);
    }
    break;

    case Opcode::Store:
    {
        uint8_t funct3;
        readFieldsS(encoded, instr, funct3);
        decodeStore(instr, funct3);
    }
    break;

    case Opcode::OpImm:
    {
        uint8_t funct3;
        readFieldsI(encoded, instr, funct3);
        decodeOpImm(instr, funct3);
    }
    break;

    case Opcode::Op:
    {
        uint8_t funct3, funct7;
        readFieldsR(encoded, instr, funct3, funct7);
        decodeOp(instr, funct3, funct7);
    }
    break;

    default:
        throw std::runtime_error(
            "Illegal instruction opcode: "
            + std::to_string(static_cast<uint8_t>(getOpcode(encoded)))
        );
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
        uint32_t encoded = loadU32(program.data() + programOffset);
        Instruction instruction {};
        decodeInstruction(instruction, encoded);
        instructions.push_back(instruction);
    }

    return instructions;
}

} // namespace riscv
