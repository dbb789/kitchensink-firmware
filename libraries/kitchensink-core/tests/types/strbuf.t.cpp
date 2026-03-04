#include "gtest/gtest.h"

#include "types/strbuf.h"

TEST(StrBuf, Empty)
{
    StrBuf<12> str;

    ASSERT_EQ(str, "");
    ASSERT_EQ(str.capacity(), 11);
    ASSERT_EQ(str.length(), 0);
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(str[0], '\0');
    ASSERT_EQ(*(str.begin()), '\0');
    ASSERT_EQ(str.begin(), str.end());
}

TEST(StrBuf, Construct)
{
    StrBuf<12> str("test");

    ASSERT_EQ(str, "test");
    ASSERT_EQ(str.capacity(), 11);
    ASSERT_EQ(str.length(), 4);
    ASSERT_FALSE(str.empty());
    
    ASSERT_EQ(str[0], 't');
    ASSERT_EQ(str[1], 'e');
    ASSERT_EQ(str[2], 's');
    ASSERT_EQ(str[3], 't');
    ASSERT_EQ(str[4], '\0');

    auto it(str.begin());
    
    ASSERT_EQ(*it, 't');
    ASSERT_EQ(*(++it), 'e');
    ASSERT_EQ(*(++it), 's');
    ASSERT_EQ(*(++it), 't');
    ASSERT_EQ(*(++it), '\0');
    ASSERT_EQ(it, str.end());
}

TEST(StrBuf, ConstructOverflow)
{
    StrBuf<4> str("abcdefg");

    ASSERT_EQ(str, "abc");
    ASSERT_EQ(str.capacity(), 3);
    ASSERT_EQ(str.length(), 3);
    ASSERT_FALSE(str.empty());
    
    ASSERT_EQ(str[0], 'a');
    ASSERT_EQ(str[1], 'b');
    ASSERT_EQ(str[2], 'c');
    ASSERT_EQ(str[3], '\0');

    auto it(str.begin());

    ASSERT_EQ(*it, 'a');
    ASSERT_EQ(*(++it), 'b');
    ASSERT_EQ(*(++it), 'c');
    ASSERT_EQ(*(++it), '\0');
    ASSERT_EQ(it, str.end());
}

TEST(StrBuf, Clear)
{
    StrBuf<12> str("test");

    str.clear();

    ASSERT_EQ(str, "");
    ASSERT_EQ(str.capacity(), 11);
    ASSERT_EQ(str.length(), 0);
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(str[0], '\0');
    ASSERT_EQ(*(str.begin()), '\0');
    ASSERT_EQ(str.begin(), str.end());
}

TEST(StrBuf, ClearSecure)
{
    StrBuf<12> str("test");

    str[8] = 'A';
    
    str.clearSecure();

    const char* data = reinterpret_cast<const char*>(str.begin());

    for (std::size_t i(0); i < 12; ++i)
    {
        ASSERT_EQ(data[i], '\0');
    }
}

TEST(StrBuf, InsertBegin)
{
    StrBuf<12> str("test");

    str.insert(str.begin(), '!');

    ASSERT_EQ(str.length(), 5);
    ASSERT_FALSE(str.empty());
    
    ASSERT_EQ(str[0], '!');
    ASSERT_EQ(str[1], 't');
    ASSERT_EQ(str[2], 'e');
    ASSERT_EQ(str[3], 's');
    ASSERT_EQ(str[4], 't');
    ASSERT_EQ(str[5], '\0');

    auto it(str.begin());
    
    ASSERT_EQ(*it, '!');
    ASSERT_EQ(*(++it), 't');
    ASSERT_EQ(*(++it), 'e');
    ASSERT_EQ(*(++it), 's');
    ASSERT_EQ(*(++it), 't');
    ASSERT_EQ(*(++it), '\0');
    ASSERT_EQ(it, str.end());
}

TEST(StrBuf, InsertMid)
{
    StrBuf<12> str("tet!");

    str.insert(str.begin() + 2, 's');

    ASSERT_EQ(str.length(), 5);
    ASSERT_FALSE(str.empty());
    
    ASSERT_EQ(str[0], 't');
    ASSERT_EQ(str[1], 'e');
    ASSERT_EQ(str[2], 's');
    ASSERT_EQ(str[3], 't');
    ASSERT_EQ(str[4], '!');
    ASSERT_EQ(str[5], '\0');

    auto it(str.begin());
    
    ASSERT_EQ(*it, 't');
    ASSERT_EQ(*(++it), 'e');
    ASSERT_EQ(*(++it), 's');
    ASSERT_EQ(*(++it), 't');
    ASSERT_EQ(*(++it), '!');
    ASSERT_EQ(*(++it), '\0');
    ASSERT_EQ(it, str.end());
}

TEST(StrBuf, InsertEnd)
{
    StrBuf<12> str("test");

    str.insert(str.end(), '!');

    ASSERT_EQ(str.length(), 5);
    ASSERT_FALSE(str.empty());
    
    ASSERT_EQ(str[0], 't');
    ASSERT_EQ(str[1], 'e');
    ASSERT_EQ(str[2], 's');
    ASSERT_EQ(str[3], 't');
    ASSERT_EQ(str[4], '!');
    ASSERT_EQ(str[5], '\0');

    auto it(str.begin());
    
    ASSERT_EQ(*it, 't');
    ASSERT_EQ(*(++it), 'e');
    ASSERT_EQ(*(++it), 's');
    ASSERT_EQ(*(++it), 't');
    ASSERT_EQ(*(++it), '!');
    ASSERT_EQ(*(++it), '\0');
    ASSERT_EQ(it, str.end());
}

TEST(StrBuf, InsertOverflow)
{
    StrBuf<4> str("abc");

    str.insert(str.begin(), 'd');

    ASSERT_EQ(str, "abc");
}

TEST(StrBuf, EraseMid)
{
    StrBuf<12> str("test!");

    str.erase(str.begin() + 2);

    ASSERT_EQ(str.length(), 4);
    ASSERT_FALSE(str.empty());
    
    ASSERT_EQ(str[0], 't');
    ASSERT_EQ(str[1], 'e');
    ASSERT_EQ(str[2], 't');
    ASSERT_EQ(str[3], '!');
    ASSERT_EQ(str[4], '\0');

    auto it(str.begin());
    
    ASSERT_EQ(*it, 't');
    ASSERT_EQ(*(++it), 'e');
    ASSERT_EQ(*(++it), 't');
    ASSERT_EQ(*(++it), '!');
    ASSERT_EQ(*(++it), '\0');
    ASSERT_EQ(it, str.end());
}

TEST(StrBuf, EraseEnd)
{
    StrBuf<12> str("test!");

    str.erase(str.end() - 1);

    ASSERT_EQ(str.length(), 4);
    ASSERT_FALSE(str.empty());
    
    ASSERT_EQ(str[0], 't');
    ASSERT_EQ(str[1], 'e');
    ASSERT_EQ(str[2], 's');
    ASSERT_EQ(str[3], 't');
    ASSERT_EQ(str[4], '\0');

    auto it(str.begin());
    
    ASSERT_EQ(*it, 't');
    ASSERT_EQ(*(++it), 'e');
    ASSERT_EQ(*(++it), 's');
    ASSERT_EQ(*(++it), 't');
    ASSERT_EQ(*(++it), '\0');
    ASSERT_EQ(it, str.end());
}

TEST(StrBuf, EraseAllForward)
{
    StrBuf<12> str("test");

    for (int i = 0; i < 4; ++i)
    {
        str.erase(str.begin());
    }

    ASSERT_EQ(str, "");
    ASSERT_EQ(str.capacity(), 11);
    ASSERT_EQ(str.length(), 0);
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(str[0], '\0');
    ASSERT_EQ(*(str.begin()), '\0');
    ASSERT_EQ(str.begin(), str.end());
}

TEST(StrBuf, EraseAllBackward)
{
    StrBuf<12> str("test");

    for (int i = 0; i < 4; ++i)
    {
        str.erase(str.end() - 1);
    }

    ASSERT_EQ(str, "");
    ASSERT_EQ(str.capacity(), 11);
    ASSERT_EQ(str.length(), 0);
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(str[0], '\0');
    ASSERT_EQ(*(str.begin()), '\0');
    ASSERT_EQ(str.begin(), str.end());
}
