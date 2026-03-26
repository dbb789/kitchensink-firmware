#include "gtest/gtest.h"

#include "types/dataref.h"
#include "crypto/cryptotypes.h"
#include "crypto/hmaccontext.h"
#include "testutil/testutil.h"

// NOTE: HMACs generated using python3 -c "import hmac, hashlib; print(hmac.new(key, data, hashlib.sha256).hexdigest())"

TEST(HMACContext, Generate_0)
{
    Crypto::Key key;
    TestUtil::hexToArray("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20", key);

    DataRef data("abc");

    Crypto::HMAC hmac;

    ASSERT_TRUE(HMACContext::generate(key, data, hmac));

    Crypto::HMAC expected;
    TestUtil::hexToArray("a21b1f5d4cf4f73a4dd939750f7a066a7f98cc131cb16a6692759021cfab8181", expected);

    ASSERT_EQ(hmac, expected);
}

TEST(HMACContext, Generate_1)
{
    Crypto::Key key;
    TestUtil::hexToArray("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20", key);

    DataRef data("The quick brown fox jumps over the lazy dog");

    Crypto::HMAC hmac;

    ASSERT_TRUE(HMACContext::generate(key, data, hmac));

    Crypto::HMAC expected;
    TestUtil::hexToArray("6d0921ae608291413d14cf2d0a1329464c08f19c9f46100d09c3a8e6bcb7a22c", expected);

    ASSERT_EQ(hmac, expected);
}

TEST(HMACContext, Generate_2)
{
    Crypto::Key key;
    TestUtil::hexToArray("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20", key);

    DataRef data("");

    Crypto::HMAC hmac;

    ASSERT_TRUE(HMACContext::generate(key, data, hmac));

    Crypto::HMAC expected;
    TestUtil::hexToArray("462476a897ddfdbd40d1420e08a5bcfeeb25c3e2ade6a0a9083b327b9ef9fca1", expected);

    ASSERT_EQ(hmac, expected);
}

TEST(HMACContext, Generate_DifferentKey)
{
    Crypto::Key key;
    TestUtil::hexToArray("4141414141414141414141414141414141414141414141414141414141414141", key);

    DataRef data("abc");

    Crypto::HMAC hmac;

    ASSERT_TRUE(HMACContext::generate(key, data, hmac));

    Crypto::HMAC expected;
    TestUtil::hexToArray("7e2b3bd5ad0e797ae29d038e5146f5adb8487509c2208d8ca89e70681afd0a6f", expected);

    ASSERT_EQ(hmac, expected);
}

TEST(HMACContext, MultiUpdate_0)
{
    Crypto::Key key;
    TestUtil::hexToArray("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20", key);

    HMACContext ctx;

    ASSERT_TRUE(ctx.init(key));
    ASSERT_TRUE(ctx.update(DataRef("Hello, ")));
    ASSERT_TRUE(ctx.update(DataRef("World!")));

    Crypto::HMAC hmac;

    ASSERT_TRUE(ctx.finish(hmac));

    Crypto::HMAC expected;
    TestUtil::hexToArray("07f1bfcb5a28b0f45275a72907b0e84521f6efe9d4fde3dbe5ed5ece1a998b9e", expected);

    ASSERT_EQ(hmac, expected);
}

TEST(HMACContext, MultiUpdate_1)
{
    Crypto::Key key;
    TestUtil::hexToArray("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20", key);

    HMACContext ctx;

    ASSERT_TRUE(ctx.init(key));
    ASSERT_TRUE(ctx.update(DataRef("abc")));
    ASSERT_TRUE(ctx.update(DataRef("def")));
    ASSERT_TRUE(ctx.update(DataRef("ghi")));

    Crypto::HMAC hmac;

    ASSERT_TRUE(ctx.finish(hmac));

    Crypto::HMAC expected;
    TestUtil::hexToArray("d0c834a429cb376dcea179faa089a6f663eb515d75024844f0b1498191885761", expected);

    ASSERT_EQ(hmac, expected);
}

TEST(HMACContext, MultiUpdateMatchesGenerate)
{
    Crypto::Key key;
    TestUtil::hexToArray("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20", key);

    const DataRef part1("The quick brown fox jumps over the lazy dog. ");

    // Build expected via generate() over the concatenated string.
    const DataRef fullData("The quick brown fox jumps over the lazy dog. "
                           "The quick brown fox jumps over the lazy dog. "
                           "The quick brown fox jumps over the lazy dog. "
                           "The quick brown fox jumps over the lazy dog. "
                           "The quick brown fox jumps over the lazy dog. ");

    Crypto::HMAC generateHmac;
    ASSERT_TRUE(HMACContext::generate(key, fullData, generateHmac));

    HMACContext ctx;
    ASSERT_TRUE(ctx.init(key));
    ASSERT_TRUE(ctx.update(part1));
    ASSERT_TRUE(ctx.update(part1));
    ASSERT_TRUE(ctx.update(part1));
    ASSERT_TRUE(ctx.update(part1));
    ASSERT_TRUE(ctx.update(part1));

    Crypto::HMAC updateHmac;
    ASSERT_TRUE(ctx.finish(updateHmac));

    ASSERT_EQ(generateHmac, updateHmac);
}
