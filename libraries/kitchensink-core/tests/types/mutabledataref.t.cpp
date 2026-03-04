#include "gtest/gtest.h"

#include "types/mutabledataref.h"

#include <array>
#include <cstdint>

TEST(MutableDataRef, Empty)
{
    MutableDataRef d;

    ASSERT_EQ(d.size(), 0);
    ASSERT_EQ(d.begin(), d.end());
}

TEST(MutableDataRef, FromIteratorRange)
{
    uint8_t data[] = {1, 2, 3, 4};
    MutableDataRef d(data, data + 4);

    ASSERT_EQ(d.size(), 4);
    ASSERT_EQ(d[0], 1);
    ASSERT_EQ(d[1], 2);
    ASSERT_EQ(d[2], 3);
    ASSERT_EQ(d[3], 4);
}

TEST(MutableDataRef, FromArray)
{
    std::array<uint8_t, 4> arr = {{10, 20, 30, 40}};
    MutableDataRef d(arr);

    ASSERT_EQ(d.size(), 4);
    ASSERT_EQ(d[0], 10);
    ASSERT_EQ(d[1], 20);
    ASSERT_EQ(d[2], 30);
    ASSERT_EQ(d[3], 40);
}

TEST(MutableDataRef, Modify)
{
    std::array<uint8_t, 4> arr = {{1, 2, 3, 4}};
    MutableDataRef d(arr);

    d[2] = 99;

    ASSERT_EQ(arr[2], 99);
    ASSERT_EQ(d[2], 99);
}

TEST(MutableDataRef, SubsetFromStart)
{
    std::array<uint8_t, 4> arr = {{1, 2, 3, 4}};
    MutableDataRef d(arr);

    auto sub(d.subset(2));

    ASSERT_EQ(sub.size(), 2);
    ASSERT_EQ(sub[0], 3);
    ASSERT_EQ(sub[1], 4);
}

TEST(MutableDataRef, SubsetWithLen)
{
    std::array<uint8_t, 4> arr = {{1, 2, 3, 4}};
    MutableDataRef d(arr);

    auto sub(d.subset(1, 2));

    ASSERT_EQ(sub.size(), 2);
    ASSERT_EQ(sub[0], 2);
    ASSERT_EQ(sub[1], 3);
}

TEST(MutableDataRef, SubsetLenOverflow)
{
    std::array<uint8_t, 4> arr = {{1, 2, 3, 4}};
    MutableDataRef d(arr);

    auto sub(d.subset(2, 10));

    ASSERT_EQ(sub.size(), 2);
    ASSERT_EQ(sub[0], 3);
    ASSERT_EQ(sub[1], 4);
}

TEST(MutableDataRef, SubsetStartOverflow)
{
    std::array<uint8_t, 4> arr = {{1, 2, 3, 4}};
    MutableDataRef d(arr);

    auto sub(d.subset(10));

    ASSERT_EQ(sub.size(), 0);
}

TEST(MutableDataRef, SubsetWithLenStartOverflow)
{
    std::array<uint8_t, 4> arr = {{1, 2, 3, 4}};
    MutableDataRef d(arr);

    auto sub(d.subset(10, 2));

    ASSERT_EQ(sub.size(), 0);
}

TEST(MutableDataRef, Equality)
{
    std::array<uint8_t, 3> arr1 = {{1, 2, 3}};
    std::array<uint8_t, 3> arr2 = {{1, 2, 3}};
    MutableDataRef a(arr1);
    MutableDataRef b(arr2);

    ASSERT_EQ(a, b);
}

TEST(MutableDataRef, Inequality)
{
    std::array<uint8_t, 3> arr1 = {{1, 2, 3}};
    std::array<uint8_t, 3> arr2 = {{1, 2, 4}};
    MutableDataRef a(arr1);
    MutableDataRef b(arr2);

    ASSERT_NE(a, b);
}

TEST(MutableDataRef, InequalitySize)
{
    std::array<uint8_t, 3> arr1 = {{1, 2, 3}};
    std::array<uint8_t, 2> arr2 = {{1, 2}};
    MutableDataRef a(arr1);
    MutableDataRef b(arr2);

    ASSERT_NE(a, b);
}
