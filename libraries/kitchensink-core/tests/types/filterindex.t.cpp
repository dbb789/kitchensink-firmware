#include "gtest/gtest.h"

#include "types/filterindex.h"

TEST(FilterIndex, DefaultAllElements)
{
    FilterIndex<5> fi;

    ASSERT_EQ(fi.filteredSize(), 5);
    ASSERT_EQ(fi[0], 0);
    ASSERT_EQ(fi[1], 1);
    ASSERT_EQ(fi[2], 2);
    ASSERT_EQ(fi[3], 3);
    ASSERT_EQ(fi[4], 4);
}

TEST(FilterIndex, DefaultMaxElements)
{
    FilterIndex<5> fi(3);

    ASSERT_EQ(fi.filteredSize(), 3);
    ASSERT_EQ(fi[0], 0);
    ASSERT_EQ(fi[1], 1);
    ASSERT_EQ(fi[2], 2);
}

TEST(FilterIndex, FilterEven)
{
    FilterIndex<6> fi;

    fi.filter(6, [](std::size_t i) { return i % 2 == 0; });

    ASSERT_EQ(fi.filteredSize(), 3);
    ASSERT_EQ(fi[0], 0);
    ASSERT_EQ(fi[1], 2);
    ASSERT_EQ(fi[2], 4);
}

TEST(FilterIndex, FilterOdd)
{
    FilterIndex<6> fi;

    fi.filter(6, [](std::size_t i) { return i % 2 != 0; });

    ASSERT_EQ(fi.filteredSize(), 3);
    ASSERT_EQ(fi[0], 1);
    ASSERT_EQ(fi[1], 3);
    ASSERT_EQ(fi[2], 5);
}

TEST(FilterIndex, FilterNone)
{
    FilterIndex<5> fi;

    fi.filter(5, [](std::size_t) { return false; });

    ASSERT_EQ(fi.filteredSize(), 0);
}

TEST(FilterIndex, FilterAll)
{
    FilterIndex<4> fi;

    fi.filter(4, [](std::size_t) { return true; });

    ASSERT_EQ(fi.filteredSize(), 4);
    ASSERT_EQ(fi[0], 0);
    ASSERT_EQ(fi[1], 1);
    ASSERT_EQ(fi[2], 2);
    ASSERT_EQ(fi[3], 3);
}

TEST(FilterIndex, FilterWithMaxElements)
{
    FilterIndex<10> fi;

    fi.filter(4, [](std::size_t) { return true; });

    ASSERT_EQ(fi.filteredSize(), 4);
    ASSERT_EQ(fi[0], 0);
    ASSERT_EQ(fi[1], 1);
    ASSERT_EQ(fi[2], 2);
    ASSERT_EQ(fi[3], 3);
}
