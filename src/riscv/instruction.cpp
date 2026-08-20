#include <src/riscv/instruction.hpp>

#include <stdexcept>


namespace riscv
{


enum class InstructionFormat
{
    R,
    I,
    S,
    B,
    U,
    J
};


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

static uint8_t getOpcode(uint32_t encoded)
{
    return static_cast<uint8_t>(getBits(encoded, 0, 6));
}


static void decodeRFormat(uint32_t encoded, Instruction& instr, uint8_t& funct3, uint8_t& funct7)
{
    funct7 = static_cast<uint8_t>(getBits(encoded, 25, 31));
    funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14));
    instr.destinationRegister = static_cast<RegisterIndex>(getBits(encoded, 7, 11));
    instr.sourceRegister1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19));
    instr.sourceRegister2 = static_cast<RegisterIndex>(getBits(encoded, 20, 24));
};

static void decodeIFormat(uint32_t encoded, Instruction& instr, uint8_t& funct3)
{
    funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14));
    instr.destinationRegister = static_cast<RegisterIndex>(getBits(encoded, 7, 11));
    instr.sourceRegister1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19));
    instr.immediate = signExtend(getBits(encoded, 20, 31), 11);
}

static void decodeSFormat(uint32_t encoded, Instruction& instr, uint8_t& funct3)
{
    funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14));
    instr.sourceRegister1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19));
    instr.sourceRegister2 = static_cast<RegisterIndex>(getBits(encoded, 20, 24));
    instr.immediate = signExtend(getBits(encoded, 7, 11) | (getBits(encoded, 25, 31) << 5), 11);
}

static void decodeBFormat(uint32_t encoded, Instruction& instr, uint8_t& funct3)
{
    funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14));
    instr.sourceRegister1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19));
    instr.sourceRegister2 = static_cast<RegisterIndex>(getBits(encoded, 20, 24));
    instr.immediate = signExtend(
        (getBits(encoded, 8, 11) << 1) | (getBits(encoded, 25, 30) << 5)
            | (getBits(encoded, 7, 7) << 11) | (getBits(encoded, 31, 31) << 12),
        12
    );
}

static void decodeUFormat(uint32_t encoded, Instruction& instr)
{
    instr.destinationRegister = static_cast<RegisterIndex>(getBits(encoded, 7, 11));
    instr.immediate = getBits(encoded, 12, 31) << 12;
}

static void decodeJFormat(uint32_t encoded, Instruction& instr)
{
    instr.destinationRegister = static_cast<RegisterIndex>(getBits(encoded, 7, 11));
    instr.immediate = signExtend(
        (getBits(encoded, 21, 30) << 1) | (getBits(encoded, 20, 20) << 11)
            | (getBits(encoded, 12, 19) << 12) | (getBits(encoded, 31, 31) << 20),
        20
    );
}


static InstructionFormat getInstructionFormat(uint8_t opcode)
{
    switch (opcode)
    {
    case 0b0110111: // LUI
    case 0b0010111: // AUIPC
        return InstructionFormat::U;
    case 0b1101111: // JAL
        return InstructionFormat::J;
    case 0b1100111: // JALR
    case 0b0000011: // LOAD
    case 0b0010011: // OP-IMM
    case 0b1110011: // SYSTEM
        return InstructionFormat::I;
    case 0b0100011: // STORE
        return InstructionFormat::S;
    case 0b1100011: // BRANCH
        return InstructionFormat::B;
    case 0b0110011: // OP
        return InstructionFormat::R;
    default:
        throw std::runtime_error("Illegal instruction opcode");
    }
}

static Instruction::Type getInstructionType(uint8_t opcode, uint8_t funct3, uint8_t funct7)
{
    switch (opcode)
    {
    case 0b0110111: // LUI
        return Instruction::Type::LUI;
    case 0b0010111: // AUIPC
        return Instruction::Type::AUIPC;
    case 0b1101111: // JAL
        return Instruction::Type::JAL;
    case 0b1100111: // JALR
        return Instruction::Type::JALR;
    case 0b1100011: // BRANCH
        switch (funct3)
        {
        case 0b000:
            return Instruction::Type::BEQ;
        case 0b001:
            return Instruction::Type::BNE;
        case 0b100:
            return Instruction::Type::BLT;
        case 0b101:
            return Instruction::Type::BGE;
        case 0b110:
            return Instruction::Type::BLTU;
        case 0b111:
            return Instruction::Type::BGEU;
        default:
            throw std::runtime_error("Illegal instruction funct3");
        }
    case 0b0000011: // LOAD
        switch (funct3)
        {
        case 0b000:
            return Instruction::Type::LB;
        case 0b001:
            return Instruction::Type::LH;
        case 0b010:
            return Instruction::Type::LW;
        case 0b100:
            return Instruction::Type::LBU;
        case 0b101:
            return Instruction::Type::LHU;
        default:
            throw std::runtime_error("Illegal instruction funct3");
        }
    case 0b0100011: // STORE
        switch (funct3)
        {
        case 0b000:
            return Instruction::Type::SB;
        case 0b001:
            return Instruction::Type::SH;
        case 0b010:
            return Instruction::Type::SW;
        default:
            throw std::runtime_error("Illegal instruction funct3");
        }
    case 0b0010011: // OP-IMM
        switch (funct3)
        {
        case 0b000:
            return Instruction::Type::ADDI;
        case 0b010:
            return Instruction::Type::SLTI;
        case 0b011:
            return Instruction::Type::SLTIU;
        case 0b100:
            return Instruction::Type::XORI;
        case 0b110:
            return Instruction::Type::ORI;
        case 0b111:
            return Instruction::Type::ANDI;
        case 0b001:
            return Instruction::Type::SLLI;
        case 0b101:
            switch (funct7)
            {
            case 0b0000000:
                return Instruction::Type::SRLI;
            case 0b0100000:
                return Instruction::Type::SRAI;
            default:
                throw std::runtime_error("Illegal instruction funct7");
            }
        default:
            throw std::runtime_error("Illegal instruction funct3");
        }
    case 0b0110011: // OP
        switch (funct3)
        {
        case 0b000:
            switch (funct7)
            {
            case 0b0000000:
                return Instruction::Type::ADD;
            case 0b0100000:
                return Instruction::Type::SUB;
            default:
                throw std::runtime_error("Illegal instruction funct7");
            }

        case 0b001:
            return Instruction::Type::SLL;
        case 0b010:
            return Instruction::Type::SLT;
        case 0b011:
            return Instruction::Type::SLTU;
        case 0b100:
            return Instruction::Type::XOR;
        case 0b101:
            switch (funct7)
            {
            case 0b0000000:
                return Instruction::Type::SRL;
            case 0b0100000:
                return Instruction::Type::SRA;
            default:
                throw std::runtime_error("Illegal instruction funct7");
            }
        }


    default:
        throw std::runtime_error("Illegal instruction funct3");
    }
}


static Instruction decodeInstruction(uint32_t encoded)
{
    Instruction instr {};
    uint8_t funct3 = 0;
    uint8_t funct7 = 0;
    uint8_t opcode = getOpcode(encoded);

    switch (getInstructionFormat(opcode))
    {
    case InstructionFormat::U:
        decodeUFormat(encoded, instr);
        break;
    case InstructionFormat::J:
        decodeJFormat(encoded, instr);
        break;
    case InstructionFormat::I:
        decodeIFormat(encoded, instr, funct3);
        break;
    case InstructionFormat::S:
        decodeSFormat(encoded, instr, funct3);
        break;
    case InstructionFormat::B:
        decodeBFormat(encoded, instr, funct3);
        break;
    case InstructionFormat::R:
        decodeRFormat(encoded, instr, funct3, funct7);
        break;
    default:
        throw std::runtime_error("Illegal instruction format");
    }

    instr.type = static_cast<Instruction::Type>(getInstructionType(opcode, funct3, funct7));

    return instr;
}


std::vector<Instruction> decodeInstructions(std::span<uint8_t const> program)
{
    if (program.size() % 4 != 0)
    {
        throw std::runtime_error("Program size must be a multiple of 4");
    }

    std::vector<Instruction> instructions {};

    for (Pointer programOffset = 0; programOffset < program.size(); programOffset += 4)
    {
        uint32_t encoded = loadU32(program.data() + programOffset);
        Instruction instruction = decodeInstruction(encoded);
        instructions.push_back(instruction);
    }

    return instructions;
}

} // namespace riscv
