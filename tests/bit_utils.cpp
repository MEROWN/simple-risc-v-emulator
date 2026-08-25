#include <gtest/gtest.h>

#include <src/riscv/utils/bits.hpp>


using namespace riscv;

TEST(BitUtils, SingleBitManipulation)
{
    uint8_t value = 0b1000'0000;
    EXPECT_TRUE(hasBit(value, 7));
    EXPECT_FALSE(hasBit(value, 6));
    EXPECT_FALSE(hasBit(value, 0));

    setBit(value, 0);
    EXPECT_EQ(value, 0b1000'0001);

    clearBit(value, 7);
    EXPECT_EQ(value, 0b0000'0001);
}

TEST(BitUtils, OneBits)
{
    EXPECT_EQ(oneBits<uint8_t>(0), 0b0000'0000);
    EXPECT_EQ(oneBits<uint8_t>(1), 0b0000'0001);
    EXPECT_EQ(oneBits<uint8_t>(3), 0b0000'0111);
    EXPECT_EQ(oneBits<uint8_t>(8), 0b1111'1111);
}

TEST(BitUtils, GetBits)
{
    uint8_t value = 0b1011'0101;
    EXPECT_EQ(getBits(value, 0, 3), 0b0101);
    EXPECT_EQ(getBits(value, 4, 7), 0b1011);
    EXPECT_EQ(getBits(value, 2, 5), 0b1101);
}

TEST(BitUtils, SignExtend)
{
    EXPECT_EQ(signExtend<uint8_t>(0b0001'0101, 4), (int8_t) 0b1111'0101);
    EXPECT_EQ(signExtend<uint8_t>(0b0110'0101, 4), (int8_t) 0b0000'0101);

    EXPECT_EQ(signExtend<uint8_t>(0b1001'0101, 7), (int8_t) 0b1001'0101);
    EXPECT_EQ(signExtend<uint8_t>(0b0110'0101, 7), (int8_t) 0b0110'0101);

    EXPECT_EQ(signExtend<uint8_t>(0b0000'0001, 0), (int8_t) 0b1111'1111);
    EXPECT_EQ(signExtend<uint8_t>(0b0000'0000, 0), (int8_t) 0b0000'0000);
}
