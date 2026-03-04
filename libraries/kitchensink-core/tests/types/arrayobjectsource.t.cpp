#include "gtest/gtest.h"

#include "types/arrayobjectsource.h"

TEST(ArrayObjectSource, Empty)
{
    const int data[] = {1};
    ArrayObjectSource<int> source(data, data);

    ASSERT_EQ(source.size(), 0);
}

TEST(ArrayObjectSource, Size)
{
    const int data[] = {1, 2, 3, 4, 5};
    ArrayObjectSource<int> source(data, data + 5);

    ASSERT_EQ(source.size(), 5);
}

TEST(ArrayObjectSource, Access)
{
    const int data[] = {10, 20, 30};
    ArrayObjectSource<int> source(data, data + 3);

    ASSERT_EQ(source[0], 10);
    ASSERT_EQ(source[1], 20);
    ASSERT_EQ(source[2], 30);
}
