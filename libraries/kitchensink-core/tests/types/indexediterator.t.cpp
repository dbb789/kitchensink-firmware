#include "gtest/gtest.h"

#include "types/indexediterator.h"

#include <array>

TEST(IndexedIterator, Dereference)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 0);

    ASSERT_EQ(*it, 10);
}

TEST(IndexedIterator, PreIncrement)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 0);

    ASSERT_EQ(*it, 10);
    ASSERT_EQ(*(++it), 20);
    ASSERT_EQ(*(++it), 30);
}

TEST(IndexedIterator, PostIncrement)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 0);

    auto prev(it++);

    ASSERT_EQ(*prev, 10);
    ASSERT_EQ(*it, 20);
}

TEST(IndexedIterator, PreDecrement)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 2);

    ASSERT_EQ(*it, 30);
    ASSERT_EQ(*(--it), 20);
    ASSERT_EQ(*(--it), 10);
}

TEST(IndexedIterator, PostDecrement)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 2);

    auto prev(it--);

    ASSERT_EQ(*prev, 30);
    ASSERT_EQ(*it, 20);
}

TEST(IndexedIterator, EqualitySameIndex)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> a(data, 2);
    IndexedIterator<std::array<int, 5>> b(data, 2);

    ASSERT_EQ(a, b);
}

TEST(IndexedIterator, InequalityDifferentIndex)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> a(data, 1);
    IndexedIterator<std::array<int, 5>> b(data, 3);

    ASSERT_NE(a, b);
}

TEST(IndexedIterator, AddOffset)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 1);

    auto result(it + 2);

    ASSERT_EQ(*result, 40);
}

TEST(IndexedIterator, SubtractOffset)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 4);

    auto result(it - 3);

    ASSERT_EQ(*result, 20);
}

TEST(IndexedIterator, Modify)
{
    std::array<int, 5> data = {{10, 20, 30, 40, 50}};
    IndexedIterator<std::array<int, 5>> it(data, 2);

    *it = 99;

    ASSERT_EQ(data[2], 99);
}
