#include "gtest/gtest.h"

#include "types/stroutstream.h"

TEST(StrOutStream, Empty)
{
    StrBuf<12> buf;
    StrOutStream os(buf);
    
    ASSERT_EQ(os.str(), "");
    ASSERT_EQ(buf, "");
}

TEST(StrOutStream, AppendStr)
{
    StrBuf<12> buf;
    StrOutStream os(buf);
    
    os.appendStr("test");
    
    ASSERT_EQ(os.str(), "test");
    ASSERT_EQ(buf, "test");
}

TEST(StrOutStream, AppendStrMultiple)
{
    StrBuf<12> buf;
    StrOutStream os(buf);
    
    os.appendStr("te")
      .appendStr("st");
    
    ASSERT_EQ(os.str(), "test");
    ASSERT_EQ(buf, "test");
}

TEST(StrOutStream, Reset)
{
    StrBuf<12> buf("test");
    StrOutStream os(buf);
    
    os.reset();
    
    ASSERT_EQ(os.str(), "");
    ASSERT_EQ(buf, "");
}

TEST(StrOutStream, AppendStrOverflow)
{
    StrBuf<12> buf;
    StrOutStream os(buf);
    
    os.appendStr("testtesttest");
    
    ASSERT_EQ(os.str(), "testtesttes");
    ASSERT_EQ(buf, "testtesttes");
}

TEST(StrOutStream, AppendChar)
{
    StrBuf<12> buf;
    StrOutStream os(buf);
    
    os.appendChar('t')
      .appendChar('e')
      .appendChar('s')
      .appendChar('t');
    
    ASSERT_EQ(os.str(), "test");
    ASSERT_EQ(buf, "test");
}

TEST(StrOutStream, AppendCharOverflow)
{
    StrBuf<4> buf;
    StrOutStream os(buf);

    for (int i = 0; i < 8; ++i)
    {
        os.appendChar('t');
    }

    ASSERT_EQ(os.str(), "ttt");
}

TEST(StrOutStream, AppendIntSimple)
{
    StrBuf<12> buf;
    StrOutStream os(buf);
    
    os.appendInt(123);
    
    ASSERT_EQ(os.str(), "123");
    ASSERT_EQ(buf, "123");
}

TEST(StrOutStream, AppendIntOverflow)
{
    StrBuf<5> buf;
    StrOutStream os(buf);

    os.appendChar('A');
    os.appendInt(123456);
    
    ASSERT_EQ(os.str(), "A123");
    ASSERT_EQ(buf, "A123");
}

TEST(StrOutStream, AppendIntFmt)
{
    StrBuf<12> buf("Test");
    StrOutStream os(buf);
    
    os.appendInt(123, "0x%X");
    
    ASSERT_EQ(os.str(), "Test0x7B");
    ASSERT_EQ(buf, "Test0x7B");
}
