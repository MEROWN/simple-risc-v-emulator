#include <src/riscv/instruction.hpp>

#include <map>
#include <stdexcept>


namespace riscv
{


/** Returns true if the instruction encoded at given address is compressed (16-bit)
    ot not (32-bit) */
static bool isEncodingCompresssed(uint8_t const *instructionBytes)
{
    uint8_t lowestByte = instructionBytes[0];
    return (lowestByte & 0b11) != 0b11;
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

static uint8_t getOpcode(uint32_t encoded)
{
    return static_cast<uint8_t>(getBits(encoded, 0, 6));
}

static uint8_t getOpcodeCompressed(uint16_t encoded)
{
    return static_cast<uint8_t>(getBits(encoded, 0, 1));
}


struct RInstruction
{
    uint8_t funct7;
    uint8_t funct3;
    RegisterIndex rd;
    RegisterIndex rs1;
    RegisterIndex rs2;
};

static RInstruction decodeRInstruction(uint32_t encoded)
{
    return RInstruction {
        .funct7 = static_cast<uint8_t>(getBits(encoded, 25, 31)),
        .funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14)),
        .rd = static_cast<RegisterIndex>(getBits(encoded, 7, 11)),
        .rs1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19)),
        .rs2 = static_cast<RegisterIndex>(getBits(encoded, 20, 24)),
    };
}

struct IInstruction
{
    uint8_t funct3;
    RegisterIndex rd;
    RegisterIndex rs1;
    uint32_t imm;
};

static IInstruction decodeIInstruction(uint32_t encoded)
{
    return IInstruction {
        .funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14)),
        .rd = static_cast<RegisterIndex>(getBits(encoded, 7, 11)),
        .rs1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19)),
        .imm = signExtend(getBits(encoded, 20, 31), 11),
    };
}

struct SBInstruction
{
    uint8_t funct3;
    RegisterIndex rs1;
    RegisterIndex rs2;
    uint32_t imm;
};

static SBInstruction decodeSInstruction(uint32_t encoded)
{
    return SBInstruction {
        .funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14)),
        .rs1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19)),
        .rs2 = static_cast<RegisterIndex>(getBits(encoded, 20, 24)),
        .imm = signExtend(getBits(encoded, 7, 11) | (getBits(encoded, 25, 31) << 5), 11),
    };
}

static SBInstruction decodeBInstruction(uint32_t encoded)
{
    return SBInstruction {
        .funct3 = static_cast<uint8_t>(getBits(encoded, 12, 14)),
        .rs1 = static_cast<RegisterIndex>(getBits(encoded, 15, 19)),
        .rs2 = static_cast<RegisterIndex>(getBits(encoded, 20, 24)),
        .imm = signExtend(
            (getBits(encoded, 8, 11) << 1) | (getBits(encoded, 25, 30) << 5)
                | (getBits(encoded, 7, 7) << 11) | (getBits(encoded, 31, 31) << 12),
            12
        ),
    };
}

struct UJInstruction
{
    RegisterIndex rd;
    uint32_t imm;
};

static UJInstruction decodeUInstruction(uint32_t encoded)
{
    return UJInstruction {
        .rd = static_cast<RegisterIndex>(getBits(encoded, 7, 11)),
        .imm = getBits(encoded, 12, 31) << 12,
    };
}

static UJInstruction decodeJInstruction(uint32_t encoded)
{
    return UJInstruction {
        .rd = static_cast<RegisterIndex>(getBits(encoded, 7, 11)),
        .imm = signExtend(
            (getBits(encoded, 21, 30) << 1) | (getBits(encoded, 20, 20) << 11)
                | (getBits(encoded, 12, 19) << 12) | (getBits(encoded, 31, 31) << 20),
            20
        ),
    };
}


/** Returns a map from original instruction pointers to decoded instruction indexes.

    Source RISC-V instruction pointers are pointers/offsets in the program memory.
    These pointers can be directly set/added to the program counter register (PC).

    Destination pointers are just indexes/offsets in the instruction array. */
static std::map<Pointer, Pointer> calcInstructionPointerDecodingMap(std::span<uint8_t const> program
)
{
    std::map<Pointer, Pointer> pointerDecodingMap {};
    auto nextInstructionIndex = [&]() { return pointerDecodingMap.size(); };

    for (Pointer programOffset = 0; programOffset < program.size();)
    {
        pointerDecodingMap[programOffset] = nextInstructionIndex();

        if (isEncodingCompresssed(program.data() + programOffset))
            programOffset += 2;
        else
            programOffset += 4;
    }

    return pointerDecodingMap;
}


static Instruction decodeInstruction(
    uint32_t encoded, std::map<Pointer, Pointer> const& pointerDecodingMap
)
{
    switch (getOpcode(encoded))
    {
    case 0b0110111: // LUI
    {
        auto u = decodeUInstruction(encoded);
        return Instruction {
            .immediate = u.imm,
            .destinationRegister = u.rd,
            .sourceRegister1 = 0,
            .sourceRegister2 = 0,
            .type = Instruction::Type::LUI,
        };
    }

    default:
        throw std::runtime_error("Illegal instruction");
    }
}

static Instruction decodeCompressedInstruction(
    uint16_t encoded, std::map<Pointer, Pointer> const& pointerDecodingMap
)
{
    throw std::runtime_error("Compressed instruction decoding not implemented yet");
}


std::vector<Instruction> decodeInstructions(std::span<uint8_t const> program)
{
    std::vector<Instruction> instructions {};

    auto const pointerDecodingMap = calcInstructionPointerDecodingMap(program);

    for (Pointer programOffset = 0; programOffset < program.size();)
    {
        auto const instructionBytes = program.data() + programOffset;

        if (isEncodingCompresssed(instructionBytes))
        {
            uint16_t encoded = loadU16(instructionBytes);
            Instruction instruction = decodeCompressedInstruction(encoded, pointerDecodingMap);
            instructions.push_back(instruction);

            programOffset += 2;
        }
        else
        {
            uint32_t encoded = loadU32(instructionBytes);
            Instruction instruction = decodeInstruction(encoded, pointerDecodingMap);
            instructions.push_back(instruction);

            programOffset += 4;
        }
    }

    return instructions;
}

} // namespace riscv
