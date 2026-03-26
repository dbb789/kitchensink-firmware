#include "gtest/gtest.h"

#include "types/dataref.h"
#include "crypto/cryptoutil.h"
#include "testutil/testutil.h"

#include <vector>

// NOTE: Hashes generated using echo -n "<string>" | sha256sum.

TEST(CryptoUtil, SHA256_0)
{
    DataRef testData("abc");

    std::vector<uint8_t> expectedHash;
    TestUtil::hexToVector("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", expectedHash);

    Crypto::SHA256 hash;

    ASSERT_TRUE(CryptoUtil::sha256(testData.begin(), testData.end(), hash));

    ASSERT_EQ(DataRef(hash.data(), hash.data() + hash.size()),
              DataRef(expectedHash.data(), expectedHash.data() + expectedHash.size()));
}

TEST(CryptoUtil, SHA256_1)
{
    DataRef testData("This is a longer string");

    std::vector<uint8_t> expectedHash;
    TestUtil::hexToVector("5f3ee7c36a7d138e853198112b7ed0157cf72ce9cd83e65aef31233754e0df9e", expectedHash);

    Crypto::SHA256 hash;

    ASSERT_TRUE(CryptoUtil::sha256(testData.begin(), testData.end(), hash));

    ASSERT_EQ(DataRef(hash.data(), hash.data() + hash.size()),
              DataRef(expectedHash.data(), expectedHash.data() + expectedHash.size()));
}

TEST(CryptoUtil, SHA256_2)
{
    DataRef testData("This is a very long string. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. 1234567890");

    std::vector<uint8_t> expectedHash;
    TestUtil::hexToVector("4ebdb42d13c6a69f215aafa30ee6aa9f491cf052516455ab7e3ba15095027259", expectedHash);

    Crypto::SHA256 hash;

    ASSERT_TRUE(CryptoUtil::sha256(testData.begin(), testData.end(), hash));

    ASSERT_EQ(DataRef(hash.data(), hash.data() + hash.size()),
              DataRef(expectedHash.data(), expectedHash.data() + expectedHash.size()));
}

TEST(CryptoUtil, Stretch)
{
    Crypto::IV iv;
    TestUtil::hexToArray("0123456789abcdef0123456789abcdef", iv);
    
    Crypto::Key key(CryptoUtil::stretch("password", iv));

    // NOTE: Expects firmware password suffix to be _kitchenSink.
    Crypto::Key expectedKey;
    TestUtil::hexToArray("6bb03448d35a920c91ddd8ebd328051289207382168e5c784827c12bfeae9414", expectedKey);

    ASSERT_EQ(key, expectedKey);
}
