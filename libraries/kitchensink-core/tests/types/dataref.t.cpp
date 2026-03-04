#include "gtest/gtest.h"

#include "types/dataref.h"
#include "types/strref.h"

#include <array>
#include <cstdint>

TEST(DataRef, Empty)
{
    DataRef d;

    ASSERT_EQ(d.size(), 0);
    ASSERT_EQ(d.begin(), d.end());
}

TEST(DataRef, FromCString)
{
    DataRef d("test");

    ASSERT_EQ(d.size(), 4);
    ASSERT_EQ(d[0], 't');
    ASSERT_EQ(d[1], 'e');
    ASSERT_EQ(d[2], 's');
    ASSERT_EQ(d[3], 't');
}

TEST(DataRef, FromStrRef)
{
    DataRef d(StrRef("hello"));

    ASSERT_EQ(d.size(), 5);
    ASSERT_EQ(d[0], 'h');
    ASSERT_EQ(d[4], 'o');
}

TEST(DataRef, FromOctet)
{
    DataRef d(uint8_t(0xAB));

    ASSERT_EQ(d.size(), 1);
    ASSERT_EQ(d[0], 0xAB);
}

TEST(DataRef, FromIteratorRange)
{
    const uint8_t data[] = {1, 2, 3};
    DataRef d(data, data + 3);

    ASSERT_EQ(d.size(), 3);
    ASSERT_EQ(d[0], 1);
    ASSERT_EQ(d[1], 2);
    ASSERT_EQ(d[2], 3);
}

TEST(DataRef, FromArray)
{
    std::array<uint8_t, 3> arr = {{0xAA, 0xBB, 0xCC}};
    DataRef d(arr);

    ASSERT_EQ(d.size(), 3);
    ASSERT_EQ(d[0], 0xAA);
    ASSERT_EQ(d[1], 0xBB);
    ASSERT_EQ(d[2], 0xCC);
}

TEST(DataRef, Iterate)
{
    DataRef d("abc");

    auto it(d.begin());

    ASSERT_EQ(*it, 'a');
    ASSERT_EQ(*(++it), 'b');
    ASSERT_EQ(*(++it), 'c');
    ++it;
    ASSERT_EQ(it, d.end());
}

TEST(DataRef, EqualityStrings)
{
    ASSERT_EQ(DataRef("test"), DataRef("test"));
    ASSERT_NE(DataRef("test"), DataRef("diff"));
}

TEST(DataRef, EqualityDifferentLengths)
{
    ASSERT_NE(DataRef("test"), DataRef("te"));
}

TEST(DataRef, CopyConstruct)
{
    DataRef original("test");
    DataRef copy(original);

    ASSERT_EQ(copy, original);
    ASSERT_EQ(copy.size(), 4);
}

TEST(DataRef, CopyOctet)
{
    DataRef original(uint8_t(0x42));
    DataRef copy(original);

    ASSERT_EQ(copy.size(), 1);
    ASSERT_EQ(copy[0], 0x42);
    ASSERT_EQ(copy, original);
}

TEST(DataRef, Assign)
{
    DataRef a("test");
    DataRef b("other");

    b = a;

    ASSERT_EQ(b, DataRef("test"));
}

TEST(DataRef, AssignOctet)
{
    DataRef a(uint8_t(0x55));
    DataRef b;

    b = a;

    ASSERT_EQ(b.size(), 1);
    ASSERT_EQ(b[0], 0x55);
    ASSERT_EQ(b, a);
}
