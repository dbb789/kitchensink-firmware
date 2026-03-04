#include "gtest/gtest.h"

#include "types/arrayoutstream.h"
#include "types/dataref.h"
#include "types/datarefinstream.h"

#include <array>

TEST(DataRefInStream, ReadAll)
{
    DataRefInStream is(DataRef("hello"));

    std::array<uint8_t, 16> buf;
    ArrayOutStream os(buf);

    auto read(is.read(os, 5));

    ASSERT_EQ(read, 5);
    ASSERT_EQ(os.data(), DataRef("hello"));
}

TEST(DataRefInStream, ReadPartial)
{
    DataRefInStream is(DataRef("hello"));

    std::array<uint8_t, 16> buf;
    ArrayOutStream os(buf);

    auto read(is.read(os, 3));

    ASSERT_EQ(read, 3);
    ASSERT_EQ(os.data(), DataRef("hel"));
}

TEST(DataRefInStream, ReadSequential)
{
    DataRefInStream is(DataRef("hello"));

    std::array<uint8_t, 16> buf;
    ArrayOutStream os(buf);

    is.read(os, 2);
    is.read(os, 3);

    ASSERT_EQ(os.data(), DataRef("hello"));
}

TEST(DataRefInStream, ReadExhausted)
{
    DataRefInStream is(DataRef("hi"));

    std::array<uint8_t, 16> buf;
    ArrayOutStream os(buf);

    is.read(os, 2);

    auto read(is.read(os, 1));

    ASSERT_EQ(read, 0);
}

TEST(DataRefInStream, ReadBeyondEnd)
{
    DataRefInStream is(DataRef("abc"));

    std::array<uint8_t, 16> buf;
    ArrayOutStream os(buf);

    auto read(is.read(os, 10));

    ASSERT_EQ(read, 3);
    ASSERT_EQ(os.data(), DataRef("abc"));
}

TEST(DataRefInStream, ReadEmpty)
{
    DataRefInStream is(DataRef(""));

    std::array<uint8_t, 16> buf;
    ArrayOutStream os(buf);

    auto read(is.read(os, 4));

    ASSERT_EQ(read, 0);
    ASSERT_EQ(os.data().size(), 0);
}
