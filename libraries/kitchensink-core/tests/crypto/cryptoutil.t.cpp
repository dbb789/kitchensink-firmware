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


TEST(CryptoUtil, PBKDF2_PasswordTooLong)
{
    // password + suffix must not exceed Config::kPasswordMax (128 bytes)
    const std::string longPassword(120, 'a');
    const std::string longSuffix(9, 'b');   // 120 + 9 = 129 — one over the limit

    Crypto::IV  salt = {};
    Crypto::Key key;

    ASSERT_FALSE(CryptoUtil::pbkdf2HmacSha512(StrRef(longPassword.c_str()),
                                              StrRef(longSuffix.c_str()),
                                              salt,
                                              1,
                                              key));
}

TEST(CryptoUtil, PBKDF2_PasswordAtLimit)
{
    // Exactly at the limit (128 bytes) should succeed
    const std::string password(120, 'a');
    const std::string suffix(8, 'b');   // 120 + 8 = 128

    Crypto::IV  salt = {};
    Crypto::Key key;

    ASSERT_TRUE(CryptoUtil::pbkdf2HmacSha512(StrRef(password.c_str()),
                                             StrRef(suffix.c_str()),
                                             salt,
                                             1,
                                             key));
}
