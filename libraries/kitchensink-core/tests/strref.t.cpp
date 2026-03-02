#include "gtest/gtest.h"

#include "types/strref.h"

TEST(StrRef, Empty)
{
    StrRef str;

    ASSERT_EQ(str, "");
    ASSERT_EQ(str.length(), 0);
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(str[0], '\0');
    ASSERT_EQ(*(str.begin()), '\0');
    ASSERT_EQ(str.begin(), str.end());
}

TEST(StrRef, Simple)
{
    StrRef str("test");

    ASSERT_EQ(str, "test");
    ASSERT_EQ(str.length(), 4);
    ASSERT_FALSE(str.empty());
    ASSERT_EQ(str[0], 't');
    ASSERT_EQ(str[1], 'e');
    ASSERT_EQ(str[2], 's');
    ASSERT_EQ(str[3], 't');

    auto it(str.begin());

    ASSERT_EQ(*it, 't');
    ASSERT_EQ(*(++it), 'e');
    ASSERT_EQ(*(++it), 's');
    ASSERT_EQ(*(++it), 't');

    ++it;
    
    ASSERT_EQ(it, str.end());
}


TEST(StrRef, Substr)
{
    StrRef str("test");

    ASSERT_EQ(str.substr(0), "test");
    ASSERT_EQ(str.substr(1), "est");
    ASSERT_EQ(str.substr(2), "st");
    ASSERT_EQ(str.substr(3), "t");
    ASSERT_EQ(str.substr(4), "");
    ASSERT_EQ(str.substr(5), "");
}

TEST(StrRef, SubstrLen)
{
    StrRef str("test");

    ASSERT_EQ(str.substr(0, 3), "tes");
    ASSERT_EQ(str.substr(1, 3), "est");
    ASSERT_EQ(str.substr(0, 2), "te");
    ASSERT_EQ(str.substr(1, 2), "es");
    ASSERT_EQ(str.substr(2, 2), "st");
    ASSERT_EQ(str.substr(3, 2), "t");
    ASSERT_EQ(str.substr(4, 2), "");
}

TEST(StrRef, BeginsWith)
{
    StrRef str("test");

    ASSERT_TRUE(str.beginsWith("te"));
    ASSERT_TRUE(str.beginsWith("tes"));
    ASSERT_TRUE(str.beginsWith("test"));
    ASSERT_FALSE(str.beginsWith("est"));
}

TEST(StrRef, EndsWith)
{
    StrRef str("test");

    ASSERT_TRUE(str.endsWith("st"));
    ASSERT_TRUE(str.endsWith("est"));
    ASSERT_TRUE(str.endsWith("test"));
    ASSERT_FALSE(str.endsWith("tes"));
}

TEST(StrRef, BeginsWithCase)
{
    StrRef str("Test");

    ASSERT_TRUE(str.beginsWithCase("te"));
    ASSERT_TRUE(str.beginsWithCase("Tes"));
    ASSERT_TRUE(str.beginsWithCase("Test"));
    ASSERT_FALSE(str.beginsWithCase("est"));
}

TEST(StrRef, EndsWithCase)
{
    StrRef str("Test");

    ASSERT_TRUE(str.endsWithCase("st"));
    ASSERT_TRUE(str.endsWithCase("est"));
    ASSERT_TRUE(str.endsWithCase("eSt"));
    ASSERT_TRUE(str.endsWithCase("Test"));
    ASSERT_FALSE(str.endsWithCase("Tes"));
}


TEST(StrRef, TrimSimple)
{
    StrRef str("  \t test \t ");

    ASSERT_EQ(str.trim(), "test");
}

TEST(StrRef, TrimNone)
{
    StrRef str("test");

    ASSERT_EQ(str.trim(), "test");
}

TEST(StrRef, TrimAll)
{
    StrRef str("  \t \t ");

    ASSERT_EQ(str.trim(), "");
}

