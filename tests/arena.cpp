#include <src/riscv/utils/arena.hpp>

#include <gtest/gtest.h>

#include <new>

using namespace riscv;


TEST(Arena, AllocateAligned)
{
    Arena arena(16);

    auto alignedOffset = arena.getFreeOffsetAlignedTo<uint32_t>();
    auto *ptr = arena.allocate<uint32_t>(0x12345678);
    EXPECT_EQ(*ptr, 0x12345678);
    EXPECT_EQ((uint8_t *) (void *) ptr, arena.raw().data() + alignedOffset);
}


TEST(Arena, AllocationFailure)
{
    Arena arena(7);

    EXPECT_EQ(arena.getFreeSpace(), 7);
    EXPECT_EQ(arena.getFreeOffset(), 0);
    EXPECT_NE(arena.getFreeOffsetAlignedTo<uint32_t>(), arena.raw().size());

    EXPECT_EQ(arena.getFreeOffsetAlignedTo<uint64_t>(), arena.raw().size());
    EXPECT_THROW(arena.allocate<uint64_t>(), std::bad_alloc);
}
