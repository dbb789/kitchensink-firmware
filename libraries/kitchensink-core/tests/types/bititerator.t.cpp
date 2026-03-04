#include "gtest/gtest.h"

#include "types/bititerator.h"

#include <cstdint>

TEST(BitIterator, NoMore)
{
    BitIterator<uint8_t> it(0x00);

    ASSERT_FALSE(it.more());
}

TEST(BitIterator, SingleBitLow)
{
    BitIterator<uint8_t> it(0x01);

    ASSERT_TRUE(it.more());
    ASSERT_EQ(it.next(), 0);
    ASSERT_FALSE(it.more());
}

TEST(BitIterator, SingleBitHigh)
{
    BitIterator<uint8_t> it(0x80);

    ASSERT_TRUE(it.more());
    ASSERT_EQ(it.next(), 7);
    ASSERT_FALSE(it.more());
}

TEST(BitIterator, MultipleBits)
{
    BitIterator<uint8_t> it(0x0b); // bits 0, 1, 3

    ASSERT_TRUE(it.more());
    ASSERT_EQ(it.next(), 0);
    ASSERT_TRUE(it.more());
    ASSERT_EQ(it.next(), 1);
    ASSERT_TRUE(it.more());
    ASSERT_EQ(it.next(), 3);
    ASSERT_FALSE(it.more());
}

TEST(BitIterator, AllBitsSet)
{
    BitIterator<uint8_t> it(0xff);

    for (std::size_t i(0); i < 8; ++i)
    {
        ASSERT_TRUE(it.more());
        ASSERT_EQ(it.next(), i);
    }

    ASSERT_FALSE(it.more());
}

TEST(BitIterator, Uint16)
{
    BitIterator<uint16_t> it(0x0101); // bits 0, 8

    ASSERT_TRUE(it.more());
    ASSERT_EQ(it.next(), 0);
    ASSERT_TRUE(it.more());
    ASSERT_EQ(it.next(), 8);
    ASSERT_FALSE(it.more());
}
