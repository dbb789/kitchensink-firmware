#include "gtest/gtest.h"

#include "types/range.h"

#include <algorithm>
#include <iterator>

TEST(Range, ConstructFromBeginEnd)
{
    int arr[] = {1, 2, 3, 4, 5};
    Range<int*> r(arr, arr + 5);

    ASSERT_EQ(r.begin(), arr);
    ASSERT_EQ(r.end(), arr + 5);
}

TEST(Range, ConstructFromPair)
{
    int arr[] = {1, 2, 3};
    std::pair<int*, int*> p(arr, arr + 3);
    Range<int*> r(p);

    ASSERT_EQ(r.begin(), arr);
    ASSERT_EQ(r.end(), arr + 3);
}

TEST(Range, BeginEndValues)
{
    int arr[] = {10, 20, 30};
    Range<int*> r(arr, arr + 3);

    ASSERT_EQ(*r.begin(), 10);
    ASSERT_EQ(*(r.end() - 1), 30);
}

TEST(Range, MutableBeginEnd)
{
    int arr[] = {1, 2, 3};
    Range<int*> r(arr, arr + 3);

    *r.begin() = 99;

    ASSERT_EQ(arr[0], 99);
}

TEST(Range, Reverse)
{
    int arr[] = {1, 2, 3};
    Range<int*> r(arr, arr + 3);

    auto rev(r.reverse());

    ASSERT_EQ(*rev.begin(), 3);
    ASSERT_EQ(*(rev.begin() + 1), 2);
    ASSERT_EQ(*(rev.begin() + 2), 1);
}

TEST(Range, IterateForward)
{
    int arr[] = {10, 20, 30, 40};
    Range<int*> r(arr, arr + 4);

    int expected[] = {10, 20, 30, 40};

    ASSERT_TRUE(std::equal(r.begin(), r.end(), expected));
}
