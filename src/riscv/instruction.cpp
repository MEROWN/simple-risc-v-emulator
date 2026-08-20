#include <src/riscv/instruction.hpp>

#include <map>


namespace riscv
{


/** Returns true if the instruction encoded at given address is compressed (16-bit)
    ot not (32-bit) */
static bool isEncodingCompresssed(uint8_t const *instructionBytes)
{
    uint8_t lowestByte = instructionBytes[0];
    return (lowestByte & 0b11) != 0b11;
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
}

static Instruction decodeCompressedInstruction(
    uint16_t encoded, std::map<Pointer, Pointer> const& pointerDecodingMap
)
{
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
