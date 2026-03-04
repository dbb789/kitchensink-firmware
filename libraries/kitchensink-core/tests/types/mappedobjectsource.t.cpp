#include "gtest/gtest.h"

#include "types/mappedobjectsource.h"

#include <array>

TEST(MappedObjectSource, Size)
{
    std::array<int, 5> data = {{1, 2, 3, 4, 5}};
    MappedObjectSource<int, std::array<int, 5>> source(
        data, [](const int& val, std::size_t) { return val; });

    ASSERT_EQ(source.size(), 5);
}

TEST(MappedObjectSource, AccessIdentity)
{
    std::array<int, 3> data = {{10, 20, 30}};
    MappedObjectSource<int, std::array<int, 3>> source(
        data, [](const int& val, std::size_t) { return val; });

    ASSERT_EQ(source[0], 10);
    ASSERT_EQ(source[1], 20);
    ASSERT_EQ(source[2], 30);
}

TEST(MappedObjectSource, AccessTransformed)
{
    std::array<int, 3> data = {{10, 20, 30}};
    MappedObjectSource<int, std::array<int, 3>> source(
        data, [](const int& val, std::size_t) { return val * 2; });

    ASSERT_EQ(source[0], 20);
    ASSERT_EQ(source[1], 40);
    ASSERT_EQ(source[2], 60);
}

TEST(MappedObjectSource, AccessWithIndex)
{
    std::array<int, 3> data = {{10, 20, 30}};
    MappedObjectSource<int, std::array<int, 3>> source(
        data, [](const int& val, std::size_t i) { return val + int(i); });

    ASSERT_EQ(source[0], 10);
    ASSERT_EQ(source[1], 21);
    ASSERT_EQ(source[2], 32);
}
