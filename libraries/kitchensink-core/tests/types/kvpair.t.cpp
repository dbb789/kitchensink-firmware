#include "gtest/gtest.h"

#include "types/kvpair.h"

TEST(KVPair, Construct)
{
    KVPair<int, int> p(1, 42);

    ASSERT_EQ(p.key, 1);
    ASSERT_EQ(p.value, 42);
}

TEST(KVPair, ComparePairs)
{
    KVPair<int, int> a(1, 10);
    KVPair<int, int> b(2, 20);

    ASSERT_TRUE(a < b);
    ASSERT_FALSE(b < a);
}

TEST(KVPair, CompareKeyLessThanPair)
{
    KVPair<int, int> p(5, 50);

    ASSERT_TRUE(1 < p);
    ASSERT_FALSE(5 < p);
    ASSERT_FALSE(9 < p);
}

TEST(KVPair, ComparePairLessThanKey)
{
    KVPair<int, int> p(5, 50);

    ASSERT_FALSE(p < 1);
    ASSERT_FALSE(p < 5);
    ASSERT_TRUE(p < 9);
}
