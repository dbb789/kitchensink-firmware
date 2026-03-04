#include "gtest/gtest.h"

#include "types/bitmask2d.h"

#include <algorithm>

TEST(Bitmask2d, Default)
{
    Bitmask2d<4, 3> b;

    ASSERT_TRUE(b.empty());
}

TEST(Bitmask2d, SetAndRead)
{
    Bitmask2d<4, 3> b;

    b[1][2] = true;

    ASSERT_FALSE(b.empty());
    ASSERT_TRUE(b[1][2]);
    ASSERT_FALSE(b[0][0]);
    ASSERT_FALSE(b[2][0]);
}

TEST(Bitmask2d, Clear)
{
    Bitmask2d<4, 3> b;

    b[0][0] = true;
    b[2][3] = true;

    ASSERT_FALSE(b.empty());

    b.clear();

    ASSERT_TRUE(b.empty());
    ASSERT_FALSE(b[0][0]);
    ASSERT_FALSE(b[2][3]);
}

TEST(Bitmask2d, OrAssign)
{
    Bitmask2d<4, 3> a;
    Bitmask2d<4, 3> b;

    a[0][1] = true;
    b[1][2] = true;

    a |= b;

    ASSERT_TRUE(a[0][1]);
    ASSERT_TRUE(a[1][2]);
    ASSERT_FALSE(a[0][0]);
}

TEST(Bitmask2d, AndAssign)
{
    Bitmask2d<4, 3> a;
    Bitmask2d<4, 3> b;

    a[0][1] = true;
    a[1][2] = true;
    b[1][2] = true;

    a &= b;

    ASSERT_FALSE(a[0][1]);
    ASSERT_TRUE(a[1][2]);
}

TEST(Bitmask2d, XorAssign)
{
    Bitmask2d<4, 3> a;
    Bitmask2d<4, 3> b;

    a[0][1] = true;
    a[1][2] = true;
    b[1][2] = true;
    b[2][3] = true;

    a ^= b;

    ASSERT_TRUE(a[0][1]);
    ASSERT_FALSE(a[1][2]);
    ASSERT_TRUE(a[2][3]);
}

TEST(Bitmask2d, RowCount)
{
    Bitmask2d<4, 3> b;

    ASSERT_EQ(std::distance(b.begin(), b.end()), 3);
}

TEST(Bitmask2d, ConstRowCount)
{
    const Bitmask2d<4, 3> b;

    ASSERT_EQ(std::distance(b.begin(), b.end()), 3);
}
