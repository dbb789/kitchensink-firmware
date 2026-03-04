#include "gtest/gtest.h"

#include "types/arrayutil.h"

#include <array>

TEST(ArrayUtil, CreateArray)
{
    auto arr(ArrayUtil<std::array<int, 4>>::createArray(
        [](std::size_t i) { return int(i * 2); }));

    ASSERT_EQ(arr[0], 0);
    ASSERT_EQ(arr[1], 2);
    ASSERT_EQ(arr[2], 4);
    ASSERT_EQ(arr[3], 6);
}

TEST(ArrayUtil, CreateArraySingleElement)
{
    auto arr(ArrayUtil<std::array<int, 1>>::createArray(
        [](std::size_t i) { return int(i + 99); }));

    ASSERT_EQ(arr[0], 99);
}

TEST(ArrayUtil, Split)
{
    std::array<int, 5> source = {{1, 2, 3, 4, 5}};
    std::array<int, 2> first;
    std::array<int, 3> second;

    ArrayUtil<std::array<int, 5>>::split<2>(source, first, second);

    ASSERT_EQ(first[0], 1);
    ASSERT_EQ(first[1], 2);
    ASSERT_EQ(second[0], 3);
    ASSERT_EQ(second[1], 4);
    ASSERT_EQ(second[2], 5);
}

TEST(ArrayUtil, Join)
{
    std::array<int, 2> first  = {{10, 20}};
    std::array<int, 3> second = {{30, 40, 50}};
    std::array<int, 5> dest;

    ArrayUtil<std::array<int, 5>>::join<2>(first, second, dest);

    ASSERT_EQ(dest[0], 10);
    ASSERT_EQ(dest[1], 20);
    ASSERT_EQ(dest[2], 30);
    ASSERT_EQ(dest[3], 40);
    ASSERT_EQ(dest[4], 50);
}

TEST(ArrayUtil, SplitJoinRoundTrip)
{
    std::array<int, 4> source = {{7, 8, 9, 10}};
    std::array<int, 1> first;
    std::array<int, 3> second;
    std::array<int, 4> dest;

    ArrayUtil<std::array<int, 4>>::split<1>(source, first, second);
    ArrayUtil<std::array<int, 4>>::join<1>(first, second, dest);

    ASSERT_EQ(dest[0], 7);
    ASSERT_EQ(dest[1], 8);
    ASSERT_EQ(dest[2], 9);
    ASSERT_EQ(dest[3], 10);
}
