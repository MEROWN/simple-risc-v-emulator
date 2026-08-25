#include <src/riscv/instruction.hpp>

#include <src/riscv/utils/memory.hpp>

#include <gtest/gtest.h>


using namespace riscv;

/** Represents program bytes */
using Prog = std::span<uint8_t const>;

static void decode(Instruction& outInstruction, uint32_t nativeEncoded)
{
    std::array<uint8_t, 4> buffer;
    storeU32(buffer.data(), nativeEncoded);
    auto instructions = decodeInstructions(buffer);
    EXPECT_EQ(instructions.size(), 1);
    outInstruction = instructions[0];
}


TEST(InstructionDecoding, SmokeTest)
{
    auto instructions = decodeInstructions(Prog {});
    EXPECT_EQ(instructions.size(), 0);
}

TEST(InstructionDecoding, InvalidLength)
{
    EXPECT_ANY_THROW(decodeInstructions(Prog { { 0 } }));
    EXPECT_ANY_THROW(decodeInstructions(Prog { { 0, 0 } }));
    EXPECT_ANY_THROW(decodeInstructions(Prog { { 0, 0, 0 } }));
    EXPECT_ANY_THROW(decodeInstructions(Prog { { 0, 0, 0, 0, 0 } }));
}

TEST(InstructionDecoding, UnsupportedCompressedInstructions)
{
    EXPECT_ANY_THROW(decodeInstructions(Prog { { 0b01, 0, 0, 0 } }));
    EXPECT_ANY_THROW(decodeInstructions(Prog { { 0b10, 0, 0, 0 } }));
    EXPECT_ANY_THROW(decodeInstructions(Prog { { 0b00, 0, 0, 0 } }));
}

TEST(InstructionDecoding, InstructionFormats)
{
    Instruction i;

    // R-type
    decode(i, 0b0000000'11111'00001'000'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::ADD);
    EXPECT_EQ(i.destinationRegister, 0);
    EXPECT_EQ(i.sourceRegister1, 1);
    EXPECT_EQ(i.sourceRegister2, 31);

    // I-type
    decode(i, 0b100000000001'11111'000'11111'1100111);
    EXPECT_EQ(i.type, Instruction::Type::JALR);
    EXPECT_EQ(i.destinationRegister, 31);
    EXPECT_EQ(i.sourceRegister1, 31);
    EXPECT_EQ(i.immediate, (int32_t) 0b11111111111111111111'100000000001);

    // S-type
    decode(i, 0b1000001'11111'11111'000'00001'0100011);
    EXPECT_EQ(i.type, Instruction::Type::SB);
    EXPECT_EQ(i.sourceRegister1, 31);
    EXPECT_EQ(i.sourceRegister2, 31);
    EXPECT_EQ(i.immediate, (int32_t) 0b11111111111111111111'1000001'00001);

    // B-type
    decode(i, 0b1'000001'11111'11111'000'1110'0'1100011);
    EXPECT_EQ(i.type, Instruction::Type::BEQ);
    EXPECT_EQ(i.sourceRegister1, 31);
    EXPECT_EQ(i.sourceRegister2, 31);
    EXPECT_EQ(i.immediate, (int32_t) 0b11111111111111111111'1'0'000001'1110'0);

    // U-type
    decode(i, 0b10101010101010101010'11111'0110111);
    EXPECT_EQ(i.type, Instruction::Type::LUI);
    EXPECT_EQ(i.destinationRegister, 31);
    EXPECT_EQ(i.immediate, (int32_t) 0b10101010101010101010'000000000000);

    // J-type
    decode(i, 0b1'0101010101'1'00011000'11111'1101111);
    EXPECT_EQ(i.type, Instruction::Type::JAL);
    EXPECT_EQ(i.destinationRegister, 31);
    EXPECT_EQ(i.immediate, (int32_t) 0b11111111111'1'00011000'1'0101010101'0);

    // R4-type
    decode(i, 0b11111'01'00011'00010'111'01000'1000011);
    EXPECT_EQ(i.type, Instruction::Type::FMADDD);
    EXPECT_EQ(i.destinationRegister, 8);
    EXPECT_EQ(i.floatOp.roundingMode, FloatRoundingMode::Dynamic);
    EXPECT_EQ(i.sourceRegister1, 2);
    EXPECT_EQ(i.sourceRegister2, 3);
    EXPECT_EQ(i.floatOp.sourceRegister3, 31);

    // Shift instructions
    decode(i, 0b0100000'11111'00010'101'00001'0010011);
    EXPECT_EQ(i.type, Instruction::Type::SRAI);
    EXPECT_EQ(i.destinationRegister, 1);
    EXPECT_EQ(i.sourceRegister1, 2);
    EXPECT_EQ(i.immediate, 31);

    // Atomic instructions
    decode(i, 0b00001'0'1'00011'00010'010'00001'0101111);
    EXPECT_EQ(i.type, Instruction::Type::AMOSWAPW);
    EXPECT_EQ(i.destinationRegister, 1);
    EXPECT_EQ(i.sourceRegister1, 2);
    EXPECT_EQ(i.sourceRegister2, 3);
    EXPECT_EQ(i.immediate, 0b0'1);
}

TEST(InstructionDecoding, BasicInstructions)
{
    Instruction i;

    decode(i, 0b0110111);
    EXPECT_EQ(i.type, Instruction::Type::LUI);

    decode(i, 0b0010111);
    EXPECT_EQ(i.type, Instruction::Type::AUIPC);

    decode(i, 0b1101111);
    EXPECT_EQ(i.type, Instruction::Type::JAL);

    decode(i, 0b1100111);
    EXPECT_EQ(i.type, Instruction::Type::JALR);

    decode(i, 0b0000000'00000'00000'000'00000'1100011);
    EXPECT_EQ(i.type, Instruction::Type::BEQ);

    decode(i, 0b0000000'00000'00000'111'00000'1100011);
    EXPECT_EQ(i.type, Instruction::Type::BGEU);

    decode(i, 0b0000000'00000'00000'101'00000'0000011);
    EXPECT_EQ(i.type, Instruction::Type::LHU);

    decode(i, 0b0000000'00000'00000'010'00000'0010011);
    EXPECT_EQ(i.type, Instruction::Type::SLTI);

    decode(i, 0b0000000'00000'00000'101'00000'0010011);
    EXPECT_EQ(i.type, Instruction::Type::SRLI);

    decode(i, 0b0100000'00000'00000'101'00000'0010011);
    EXPECT_EQ(i.type, Instruction::Type::SRAI);

    decode(i, 0b0000000'00000'00000'000'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::ADD);

    decode(i, 0b0100000'00000'00000'000'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::SUB);

    decode(i, 0b0000000'00000'00000'101'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::SRL);

    decode(i, 0b0100000'00000'00000'101'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::SRA);

    decode(i, 0b0000000'00000'00000'110'00000'0000011);
    EXPECT_EQ(i.type, Instruction::Type::LWU);

    decode(i, 0b0000000'00000'00000'011'00000'0000011);
    EXPECT_EQ(i.type, Instruction::Type::LD);

    decode(i, 0b0000000'00000'00000'011'00000'0100011);
    EXPECT_EQ(i.type, Instruction::Type::SD);

    decode(i, 0b0000000'00000'00000'001'00000'0010011);
    EXPECT_EQ(i.type, Instruction::Type::SLLI);

    decode(i, 0b0000000'00000'00000'101'00000'0010011);
    EXPECT_EQ(i.type, Instruction::Type::SRLI);

    decode(i, 0b0100000'00000'00000'101'00000'0010011);
    EXPECT_EQ(i.type, Instruction::Type::SRAI);

    decode(i, 0b0000000'00000'00000'000'00000'0111011);
    EXPECT_EQ(i.type, Instruction::Type::ADDW);

    decode(i, 0b0100000'00000'00000'000'00000'0111011);
    EXPECT_EQ(i.type, Instruction::Type::SUBW);

    decode(i, 0b0000000'00000'00000'101'00000'0111011);
    EXPECT_EQ(i.type, Instruction::Type::SRLW);

    decode(i, 0b0100000'00000'00000'101'00000'0111011);
    EXPECT_EQ(i.type, Instruction::Type::SRAW);
}

TEST(InstructionDecoding, MultiplyExtension)
{
    Instruction i;

    decode(i, 0b0000001'00000'00000'000'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::MUL);

    decode(i, 0b0000001'00000'00000'001'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::MULH);

    decode(i, 0b0000001'00000'00000'100'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::DIV);

    decode(i, 0b0000001'00000'00000'101'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::DIVU);

    decode(i, 0b0000001'00000'00000'110'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::REM);

    decode(i, 0b0000001'00000'00000'111'00000'0110011);
    EXPECT_EQ(i.type, Instruction::Type::REMU);

    decode(i, 0b0000001'00000'00000'000'00000'0111011);
    EXPECT_EQ(i.type, Instruction::Type::MULW);

    decode(i, 0b0000001'00000'00000'100'00000'0111011);
    EXPECT_EQ(i.type, Instruction::Type::DIVW);
}

TEST(InstructionDecoding, FenceInstructions)
{
    Instruction i;

    decode(i, 0b0000'0000'0000'00000'000'00000'0001111);
    EXPECT_EQ(i.type, Instruction::Type::FENCE);

    decode(i, 0b1000'0011'0011'00000'000'00000'0001111);
    EXPECT_EQ(i.type, Instruction::Type::FENCETSO);

    decode(i, 0b0000'0001'0000'00000'000'00000'0001111);
    EXPECT_EQ(i.type, Instruction::Type::PAUSE);

    decode(i, 0b000000000000'00000'001'00000'0001111);
    EXPECT_EQ(i.type, Instruction::Type::FENCEI);
}

TEST(InstructionDecoding, SystemInstructions)
{
    Instruction i;

    decode(i, 0b000000000000'00000'000'00000'1110011);
    EXPECT_EQ(i.type, Instruction::Type::ECALL);

    decode(i, 0b000000000001'00000'000'00000'1110011);
    EXPECT_EQ(i.type, Instruction::Type::EBREAK);

    decode(i, 0b000000000000'00000'001'00000'1110011);
    EXPECT_EQ(i.type, Instruction::Type::CSRRW);

    decode(i, 0b000000000000'00000'111'00000'1110011);
    EXPECT_EQ(i.type, Instruction::Type::CSRRCI);
}

TEST(InstructionDecoding, AtomicInstructions)
{
    Instruction i;

    decode(i, 0b00010'00'00000'00000'010'00000'0101111);
    EXPECT_EQ(i.type, Instruction::Type::LRW);

    decode(i, 0b00010'00'00000'00000'011'00000'0101111);
    EXPECT_EQ(i.type, Instruction::Type::LRD);
}

TEST(InstructionDecoding, FloatingPointInstructions)
{
    Instruction i;

    decode(i, 0b0000000'00000'00000'010'00000'0000111);
    EXPECT_EQ(i.type, Instruction::Type::FLW);

    decode(i, 0b0000000'00000'00000'010'00000'0100111);
    EXPECT_EQ(i.type, Instruction::Type::FSW);

    decode(i, 0b00000'00'00000'00000'000'00000'1000011);
    EXPECT_EQ(i.type, Instruction::Type::FMADDS);

    decode(i, 0b00000'00'00000'00000'000'00000'1000111);
    EXPECT_EQ(i.type, Instruction::Type::FMSUBS);

    decode(i, 0b00000'00'00000'00000'000'00000'1001111);
    EXPECT_EQ(i.type, Instruction::Type::FNMADDS);

    decode(i, 0b00000'00'00000'00000'000'00001'1001011);
    EXPECT_EQ(i.type, Instruction::Type::FNMSUBS);

    decode(i, 0b0000000'00000'00000'000'00000'1010011);
    EXPECT_EQ(i.type, Instruction::Type::FADDS);

    decode(i, 0b0000100'00000'00000'000'00000'1010011);
    EXPECT_EQ(i.type, Instruction::Type::FSUBS);

    decode(i, 0b1100000'00010'00000'000'00000'1010011);
    EXPECT_EQ(i.type, Instruction::Type::FCVTLS);

    decode(i, 0b1100000'00011'00000'000'00000'1010011);
    EXPECT_EQ(i.type, Instruction::Type::FCVTLUS);

    decode(i, 0b1101000'00010'00000'000'00000'1010011);
    EXPECT_EQ(i.type, Instruction::Type::FCVTSL);

    decode(i, 0b1101000'00011'00000'000'00000'1010011);
    EXPECT_EQ(i.type, Instruction::Type::FCVTSLU);


    decode(i, 0b0000000'00000'00000'011'00000'0000111);
    EXPECT_EQ(i.type, Instruction::Type::FLD);

    decode(i, 0b0000000'00000'00000'011'00000'0100111);
    EXPECT_EQ(i.type, Instruction::Type::FSD);

    decode(i, 0b00000'01'00000'00000'000'00000'1000011);
    EXPECT_EQ(i.type, Instruction::Type::FMADDD);

    decode(i, 0b00000'01'00000'00000'000'00000'1000111);
    EXPECT_EQ(i.type, Instruction::Type::FMSUBD);

    decode(i, 0b00000'01'00000'00000'000'00000'1001111);
    EXPECT_EQ(i.type, Instruction::Type::FNMADDD);

    decode(i, 0b00000'01'00000'00000'000'00000'1001011);
    EXPECT_EQ(i.type, Instruction::Type::FNMSUBD);

    decode(i, 0b0000001'00000'00000'000'00000'1010011);
    EXPECT_EQ(i.type, Instruction::Type::FADDD);
}
