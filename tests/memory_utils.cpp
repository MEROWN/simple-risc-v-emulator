#include <src/riscv/utils/memory.hpp>

#include <gtest/gtest.h>


using namespace riscv;


TEST(MemoryUtils, MemoryRead)
{
    uint8_t buffer[] = { 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01 };

    uint16_t value16 = loadU16(buffer);
    EXPECT_EQ(value16, 0xCDEF);

    uint32_t value32 = loadU32(buffer);
    EXPECT_EQ(value32, 0x89ABCDEF);

    uint64_t value64 = loadU64(buffer);
    EXPECT_EQ(value64, 0x0123456789ABCDEF);
}

TEST(MemoryUtils, MemoryWrite)
{
    uint8_t buffer[8] = { 0 };

    storeU16(buffer, 0xCDEF);
    EXPECT_EQ(buffer[0], 0xEF);
    EXPECT_EQ(buffer[1], 0xCD);

    storeU32(buffer, 0x89ABCDEF);
    EXPECT_EQ(buffer[0], 0xEF);
    EXPECT_EQ(buffer[1], 0xCD);
    EXPECT_EQ(buffer[2], 0xAB);
    EXPECT_EQ(buffer[3], 0x89);

    storeU64(buffer, 0x0123456789ABCDEF);
    EXPECT_EQ(buffer[0], 0xEF);
    EXPECT_EQ(buffer[1], 0xCD);
    EXPECT_EQ(buffer[2], 0xAB);
    EXPECT_EQ(buffer[3], 0x89);
    EXPECT_EQ(buffer[4], 0x67);
    EXPECT_EQ(buffer[5], 0x45);
    EXPECT_EQ(buffer[6], 0x23);
    EXPECT_EQ(buffer[7], 0x01);
}
