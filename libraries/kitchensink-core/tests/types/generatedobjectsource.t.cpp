#include "gtest/gtest.h"

#include "types/generatedobjectsource.h"

TEST(GeneratedObjectSource, Size)
{
    GeneratedObjectSource<int> source(5, [](std::size_t) { return 0; });

    ASSERT_EQ(source.size(), 5);
}

TEST(GeneratedObjectSource, ZeroSize)
{
    GeneratedObjectSource<int> source(0, [](std::size_t) { return 0; });

    ASSERT_EQ(source.size(), 0);
}

TEST(GeneratedObjectSource, Access)
{
    GeneratedObjectSource<int> source(4, [](std::size_t i) { return int(i * 10); });

    ASSERT_EQ(source[0], 0);
    ASSERT_EQ(source[1], 10);
    ASSERT_EQ(source[2], 20);
    ASSERT_EQ(source[3], 30);
}

TEST(GeneratedObjectSource, Constant)
{
    GeneratedObjectSource<int> source(3, [](std::size_t) { return 42; });

    ASSERT_EQ(source[0], 42);
    ASSERT_EQ(source[1], 42);
    ASSERT_EQ(source[2], 42);
}
