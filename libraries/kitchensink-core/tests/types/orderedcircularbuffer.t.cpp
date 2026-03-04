#include "gtest/gtest.h"

#include "types/orderedcircularbuffer.h"

#include <algorithm>
#include <vector>

TEST(OrderedCircularBuffer, Default)
{
    OrderedCircularBuffer<int, int, 5> ocb;

    ASSERT_TRUE(ocb.empty());
    ASSERT_EQ(ocb.size(), 0);
}

TEST(OrderedCircularBuffer, InsertOrdered)
{
    OrderedCircularBuffer<int, int, 5> ocb;

    ocb.insert(KVPair<int, int>(3, 30));
    ocb.insert(KVPair<int, int>(1, 10));
    ocb.insert(KVPair<int, int>(2, 20));

    ASSERT_FALSE(ocb.empty());
    ASSERT_EQ(ocb.size(), 3);

    ASSERT_EQ(ocb.peek().key, 1);
    ASSERT_EQ(ocb.pop().key, 1);
    ASSERT_EQ(ocb.pop().key, 2);
    ASSERT_EQ(ocb.pop().key, 3);

    ASSERT_TRUE(ocb.empty());
}

TEST(OrderedCircularBuffer, InsertAlreadyOrdered)
{
    OrderedCircularBuffer<int, int, 5> ocb;

    ocb.insert(KVPair<int, int>(1, 10));
    ocb.insert(KVPair<int, int>(2, 20));
    ocb.insert(KVPair<int, int>(3, 30));

    ASSERT_EQ(ocb.pop().key, 1);
    ASSERT_EQ(ocb.pop().key, 2);
    ASSERT_EQ(ocb.pop().key, 3);
}

TEST(OrderedCircularBuffer, InsertReverseOrder)
{
    OrderedCircularBuffer<int, int, 5> ocb;

    ocb.insert(KVPair<int, int>(5, 50));
    ocb.insert(KVPair<int, int>(4, 40));
    ocb.insert(KVPair<int, int>(3, 30));
    ocb.insert(KVPair<int, int>(2, 20));
    ocb.insert(KVPair<int, int>(1, 10));

    ASSERT_EQ(ocb.pop().key, 1);
    ASSERT_EQ(ocb.pop().key, 2);
    ASSERT_EQ(ocb.pop().key, 3);
    ASSERT_EQ(ocb.pop().key, 4);
    ASSERT_EQ(ocb.pop().key, 5);
}

TEST(OrderedCircularBuffer, PeekDoesNotRemove)
{
    OrderedCircularBuffer<int, int, 5> ocb;

    ocb.insert(KVPair<int, int>(1, 10));

    ASSERT_EQ(ocb.peek().key, 1);
    ASSERT_EQ(ocb.size(), 1);
    ASSERT_EQ(ocb.peek().key, 1);
}

TEST(OrderedCircularBuffer, Erase)
{
    OrderedCircularBuffer<int, int, 5> ocb;

    ocb.insert(KVPair<int, int>(1, 10));
    ocb.insert(KVPair<int, int>(2, 20));
    ocb.insert(KVPair<int, int>(3, 30));

    auto it(ocb.begin());
    ++it;
    ocb.erase(it);

    ASSERT_EQ(ocb.size(), 2);
    ASSERT_EQ(ocb.pop().key, 1);
    ASSERT_EQ(ocb.pop().key, 3);
}

TEST(OrderedCircularBuffer, Iterate)
{
    OrderedCircularBuffer<int, int, 5> ocb;

    ocb.insert(KVPair<int, int>(3, 30));
    ocb.insert(KVPair<int, int>(1, 10));
    ocb.insert(KVPair<int, int>(2, 20));

    std::vector<int> keys;

    for (const auto& pair : ocb)
    {
        keys.push_back(pair.key);
    }

    ASSERT_EQ(keys.size(), 3);
    ASSERT_EQ(keys[0], 1);
    ASSERT_EQ(keys[1], 2);
    ASSERT_EQ(keys[2], 3);
}
