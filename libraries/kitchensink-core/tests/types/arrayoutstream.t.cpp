#include "gtest/gtest.h"

#include "types/arrayoutstream.h"
#include "types/dataref.h"

#include <array>

TEST(ArrayOutStream, Empty)
{
    std::array<uint8_t, 8> buf;
    ArrayOutStream os(buf);

    ASSERT_EQ(os.position(), 0);
    ASSERT_EQ(os.remaining(), 8);
    ASSERT_EQ(os.data().size(), 0);
}

TEST(ArrayOutStream, Write)
{
    std::array<uint8_t, 8> buf;
    ArrayOutStream os(buf);

    auto written(os.write(DataRef("test")));

    ASSERT_EQ(written, 4);
    ASSERT_EQ(os.position(), 4);
    ASSERT_EQ(os.remaining(), 4);
    ASSERT_EQ(os.data(), DataRef("test"));
}

TEST(ArrayOutStream, WriteMultiple)
{
    std::array<uint8_t, 8> buf;
    ArrayOutStream os(buf);

    os.write(DataRef("te"));
    os.write(DataRef("st"));

    ASSERT_EQ(os.position(), 4);
    ASSERT_EQ(os.remaining(), 4);
    ASSERT_EQ(os.data(), DataRef("test"));
}

TEST(ArrayOutStream, WriteOverflow)
{
    std::array<uint8_t, 4> buf;
    ArrayOutStream os(buf);

    auto written(os.write(DataRef("testABC")));

    ASSERT_EQ(written, 4);
    ASSERT_EQ(os.position(), 4);
    ASSERT_EQ(os.remaining(), 0);
    ASSERT_EQ(os.data(), DataRef("test"));
}

TEST(ArrayOutStream, Reset)
{
    std::array<uint8_t, 8> buf;
    ArrayOutStream os(buf);

    os.write(DataRef("test"));
    os.reset();

    ASSERT_EQ(os.position(), 0);
    ASSERT_EQ(os.remaining(), 8);
    ASSERT_EQ(os.data().size(), 0);
}

TEST(ArrayOutStream, WriteAfterReset)
{
    std::array<uint8_t, 8> buf;
    ArrayOutStream os(buf);

    os.write(DataRef("first"));
    os.reset();
    os.write(DataRef("ok"));

    ASSERT_EQ(os.position(), 2);
    ASSERT_EQ(os.data(), DataRef("ok"));
}
