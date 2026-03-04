#include "gtest/gtest.h"

#include "types/arrayoutstream.h"
#include "types/dataref.h"
#include "types/datarefinstream.h"
#include "types/strref.h"
#include "serialize/iniformat.h"

#include <array>

TEST(IniFormatIStream, EmptyInput)
{
    DataRefInStream is("");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_FALSE(ini.nextSection(sectionName));
}

TEST(IniFormatIStream, SingleSection)
{
    DataRefInStream is("[config]\n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "config");
    ASSERT_FALSE(ini.nextSection(sectionName));
}

TEST(IniFormatIStream, SectionWithProperty)
{
    DataRefInStream is("[section]\nkey=value\n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "section");

    StrRef key;
    StrRef value;

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "key");
    ASSERT_EQ(value, "value");
    ASSERT_FALSE(ini.nextProperty(key, value));
}

TEST(IniFormatIStream, MultipleProperties)
{
    DataRefInStream is("[section]\nfoo=bar\nbaz=qux\n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));

    StrRef key;
    StrRef value;

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "foo");
    ASSERT_EQ(value, "bar");

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "baz");
    ASSERT_EQ(value, "qux");

    ASSERT_FALSE(ini.nextProperty(key, value));
}

TEST(IniFormatIStream, MultipleSections)
{
    DataRefInStream is("[section1]\nkey1=val1\n[section2]\nkey2=val2\n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "section1");

    StrRef key;
    StrRef value;

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "key1");
    ASSERT_EQ(value, "val1");
    ASSERT_FALSE(ini.nextProperty(key, value));

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "section2");

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "key2");
    ASSERT_EQ(value, "val2");
    ASSERT_FALSE(ini.nextProperty(key, value));

    ASSERT_FALSE(ini.nextSection(sectionName));
}

TEST(IniFormatIStream, CommentIgnored)
{
    DataRefInStream is("[section]\n; this is a comment\nkey=value\n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));

    StrRef key;
    StrRef value;

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "key");
    ASSERT_EQ(value, "value");
    ASSERT_FALSE(ini.nextProperty(key, value));
}

TEST(IniFormatIStream, WhitespaceTrimmed)
{
    DataRefInStream is("  [section]  \n  key  =  value  \n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "section");

    StrRef key;
    StrRef value;

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "key");
    ASSERT_EQ(value, "value");
}

TEST(IniFormatIStream, SectionStopsPropertyRead)
{
    DataRefInStream is("[section1]\nkey=val\n[section2]\n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "section1");

    StrRef key;
    StrRef value;

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "key");
    ASSERT_EQ(value, "val");

    // nextProperty stops at the next section header
    ASSERT_FALSE(ini.nextProperty(key, value));

    // The deferred section line is consumed by the next nextSection call
    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "section2");
}

TEST(IniFormatIStream, NoSectionBeforeProperties)
{
    DataRefInStream is("key=value\n[section]\n");
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "section");
}

TEST(IniFormatOStream, WriteSection)
{
    std::array<uint8_t, 64> buf;
    ArrayOutStream os(buf);
    IniFormat::OStream ini(os);

    ini.writeSection("config");

    ASSERT_EQ(os.data(), DataRef("\n[config]\n"));
}

TEST(IniFormatOStream, WriteProperty)
{
    std::array<uint8_t, 64> buf;
    ArrayOutStream os(buf);
    IniFormat::OStream ini(os);

    ini.writeProperty("key", "value");

    ASSERT_EQ(os.data(), DataRef("key=value\n"));
}

TEST(IniFormatOStream, WriteSectionAndProperties)
{
    std::array<uint8_t, 64> buf;
    ArrayOutStream os(buf);
    IniFormat::OStream ini(os);

    ini.writeSection("section");
    ini.writeProperty("foo", "bar");
    ini.writeProperty("baz", "qux");

    ASSERT_EQ(os.data(), DataRef("\n[section]\nfoo=bar\nbaz=qux\n"));
}

TEST(IniFormatOStream, WriteMultipleSections)
{
    std::array<uint8_t, 128> buf;
    ArrayOutStream os(buf);
    IniFormat::OStream ini(os);

    ini.writeSection("section1");
    ini.writeProperty("key1", "val1");
    ini.writeSection("section2");
    ini.writeProperty("key2", "val2");

    ASSERT_EQ(os.data(), DataRef("\n[section1]\nkey1=val1\n\n[section2]\nkey2=val2\n"));
}

TEST(IniFormatRoundTrip, SectionAndProperties)
{
    std::array<uint8_t, 256> buf;
    ArrayOutStream os(buf);

    {
        IniFormat::OStream ini(os);

        ini.writeSection("config");
        ini.writeProperty("tapDelay", "50");
        ini.writeProperty("keyRepeatDelay", "200");
    }

    DataRefInStream is(os.data());
    IniFormat::IStream ini(is);

    StrRef sectionName;

    ASSERT_TRUE(ini.nextSection(sectionName));
    ASSERT_EQ(sectionName, "config");

    StrRef key;
    StrRef value;

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "tapDelay");
    ASSERT_EQ(value, "50");

    ASSERT_TRUE(ini.nextProperty(key, value));
    ASSERT_EQ(key, "keyRepeatDelay");
    ASSERT_EQ(value, "200");

    ASSERT_FALSE(ini.nextProperty(key, value));
    ASSERT_FALSE(ini.nextSection(sectionName));
}
