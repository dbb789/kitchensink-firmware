#include "gtest/gtest.h"

#include "crypto/aesdecryptcontext.h"
#include "crypto/aesencryptcontext.h"
#include "crypto/cryptotypes.h"
#include "testutil/testutil.h"

// NOTE: Vectors generated using python3 with pycryptodome:
// AES-256-CBC, key = 0x01..0x20, iv = 0x10..0x1f

namespace
{

Crypto::Key testKey()
{
    Crypto::Key key;
    TestUtil::hexToArray("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20", key);
    return key;
}

Crypto::IV testIV()
{
    Crypto::IV iv;
    TestUtil::hexToArray("101112131415161718191a1b1c1d1e1f", iv);
    return iv;
}

}

// --- AESEncryptContext: CBC_NO_PADDING ---

TEST(AESEncryptContext, NoPadding_OneBlock)
{
    std::array<uint8_t, Crypto::kAesBlockSize> plaintext;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", plaintext);

    std::array<uint8_t, Crypto::kAesBlockSize> ciphertext;

    AESEncryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV()));
    ASSERT_TRUE(ctx.update(plaintext, ciphertext));
    ASSERT_TRUE(ctx.finish());

    std::array<uint8_t, Crypto::kAesBlockSize> expected;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6", expected);

    ASSERT_EQ(ciphertext, expected);
}

TEST(AESEncryptContext, NoPadding_ThreeBlocks)
{
    std::array<uint8_t, Crypto::kAesBlockSize> in1, in2, in3;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", in1);
    TestUtil::hexToArray("101112131415161718191a1b1c1d1e1f", in2);
    TestUtil::hexToArray("202122232425262728292a2b2c2d2e2f", in3);

    std::array<uint8_t, Crypto::kAesBlockSize> out1, out2, out3;

    AESEncryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV()));
    ASSERT_TRUE(ctx.update(in1, out1));
    ASSERT_TRUE(ctx.update(in2, out2));
    ASSERT_TRUE(ctx.update(in3, out3));
    ASSERT_TRUE(ctx.finish());

    std::array<uint8_t, Crypto::kAesBlockSize> exp1, exp2, exp3;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6", exp1);
    TestUtil::hexToArray("6dd101fac49c825c5d377e62ea7083b1", exp2);
    TestUtil::hexToArray("5a3d8adf3b57a527f89ec905aa00ed19", exp3);

    ASSERT_EQ(out1, exp1);
    ASSERT_EQ(out2, exp2);
    ASSERT_EQ(out3, exp3);
}

TEST(AESEncryptContext, NoPadding_InitTwiceFails)
{
    AESEncryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV()));
    ASSERT_FALSE(ctx.init(testKey(), testIV()));
}

TEST(AESEncryptContext, NoPadding_FinishWithoutInitFails)
{
    AESEncryptContext ctx;
    ASSERT_FALSE(ctx.finish());
}

// --- AESEncryptContext: PKCS#7 ---

TEST(AESEncryptContext, PKCS7_OneByte)
{
    std::array<uint8_t, Crypto::kAesBlockSize> plainBuf = {};
    plainBuf[0] = 0xAB;

    std::array<uint8_t, Crypto::kAesBlockSize * 2> ciphertext;
    std::size_t outLen = 0;

    AESEncryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(plainBuf, 1, plainBuf, outLen));
    ASSERT_EQ(outLen, 0u);
    ASSERT_TRUE(ctx.finish(ciphertext, outLen));
    ASSERT_EQ(outLen, Crypto::kAesBlockSize);

    std::array<uint8_t, Crypto::kAesBlockSize> expected;
    TestUtil::hexToArray("94c9f8cffbf78d72697a13147d26bcfc", expected);

    ASSERT_EQ(DataRef(ciphertext.begin(), ciphertext.begin() + outLen),
              DataRef(expected.begin(),   expected.end()));
}

TEST(AESEncryptContext, PKCS7_ExactBlock)
{
    // A full 16-byte block always produces a second all-padding block.
    std::array<uint8_t, Crypto::kAesBlockSize> plaintext;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", plaintext);

    std::array<uint8_t, Crypto::kAesBlockSize> updateOut;
    std::size_t updateLen = 0;

    std::array<uint8_t, Crypto::kAesBlockSize * 2> finishOut;
    std::size_t finishLen = 0;

    AESEncryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(plaintext, updateOut, updateLen));
    ASSERT_EQ(updateLen, Crypto::kAesBlockSize);
    ASSERT_TRUE(ctx.finish(finishOut, finishLen));
    ASSERT_EQ(finishLen, Crypto::kAesBlockSize);

    std::array<uint8_t, Crypto::kAesBlockSize * 2> expected;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6"
                         "9de10d7592d73fe22aab9f56daa0856c", expected);

    std::array<uint8_t, Crypto::kAesBlockSize * 2> actual;
    std::copy(updateOut.begin(), updateOut.begin() + updateLen, actual.begin());
    std::copy(finishOut.begin(), finishOut.begin() + finishLen, actual.begin() + updateLen);

    ASSERT_EQ(actual, expected);
}

TEST(AESEncryptContext, PKCS7_SeventeenBytes)
{
    // Block 1: bytes 0-15. Partial: byte 16 alone.
    std::array<uint8_t, Crypto::kAesBlockSize> block1In;
    std::array<uint8_t, Crypto::kAesBlockSize> partialIn = {};
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", block1In);
    partialIn[0] = 0x10;

    std::array<uint8_t, Crypto::kAesBlockSize> block1Out;
    std::size_t block1Len = 0;

    std::array<uint8_t, Crypto::kAesBlockSize> unusedOut;
    std::size_t unusedLen = 0;

    std::array<uint8_t, Crypto::kAesBlockSize * 2> finishOut;
    std::size_t finishLen = 0;

    AESEncryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(block1In, block1Out, block1Len));
    ASSERT_EQ(block1Len, Crypto::kAesBlockSize);
    ASSERT_TRUE(ctx.update(partialIn, 1, unusedOut, unusedLen));
    ASSERT_EQ(unusedLen, 0u);
    ASSERT_TRUE(ctx.finish(finishOut, finishLen));
    ASSERT_EQ(finishLen, Crypto::kAesBlockSize);

    std::array<uint8_t, Crypto::kAesBlockSize * 2> expected;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6"
                         "1a4f866ef646397234a70a5e68368514", expected);

    std::array<uint8_t, Crypto::kAesBlockSize * 2> actual;
    std::copy(block1Out.begin(), block1Out.begin() + block1Len, actual.begin());
    std::copy(finishOut.begin(), finishOut.begin() + finishLen, actual.begin() + block1Len);

    ASSERT_EQ(actual, expected);
}

TEST(AESEncryptContext, PKCS7_ThirtyTwoBytes)
{
    // Two exact blocks; finish() always emits a full padding block.
    std::array<uint8_t, Crypto::kAesBlockSize> plaintext1;
    std::array<uint8_t, Crypto::kAesBlockSize> plaintext2;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", plaintext1);
    TestUtil::hexToArray("101112131415161718191a1b1c1d1e1f", plaintext2);

    std::array<uint8_t, Crypto::kAesBlockSize> out1;
    std::array<uint8_t, Crypto::kAesBlockSize> out2;
    std::array<uint8_t, Crypto::kAesBlockSize * 2> finishOut;
    std::size_t len1 = 0, len2 = 0, finishLen = 0;

    AESEncryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(plaintext1, out1, len1));
    ASSERT_EQ(len1, Crypto::kAesBlockSize);
    ASSERT_TRUE(ctx.update(plaintext2, out2, len2));
    ASSERT_EQ(len2, Crypto::kAesBlockSize);
    ASSERT_TRUE(ctx.finish(finishOut, finishLen));
    ASSERT_EQ(finishLen, Crypto::kAesBlockSize);

    std::array<uint8_t, Crypto::kAesBlockSize * 3> expected;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6"
                         "6dd101fac49c825c5d377e62ea7083b1"
                         "b3c3557de78fb44b7dcedd7344dbeaca", expected);

    std::array<uint8_t, Crypto::kAesBlockSize * 3> actual;
    std::copy(out1.begin(),     out1.begin()     + len1,     actual.begin());
    std::copy(out2.begin(),     out2.begin()     + len2,     actual.begin() + len1);
    std::copy(finishOut.begin(), finishOut.begin() + finishLen, actual.begin() + len1 + len2);

    ASSERT_EQ(actual, expected);
}

// --- AESDecryptContext: CBC_NO_PADDING ---

TEST(AESDecryptContext, NoPadding_OneBlock)
{
    std::array<uint8_t, Crypto::kAesBlockSize> ciphertext;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6", ciphertext);

    std::array<uint8_t, Crypto::kAesBlockSize> plaintext;

    AESDecryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV()));
    ASSERT_TRUE(ctx.update(ciphertext, plaintext));
    ASSERT_TRUE(ctx.finish());

    std::array<uint8_t, Crypto::kAesBlockSize> expected;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", expected);

    ASSERT_EQ(plaintext, expected);
}

TEST(AESDecryptContext, NoPadding_ThreeBlocks)
{
    std::array<uint8_t, Crypto::kAesBlockSize * 3> ciphertext;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6"
                         "6dd101fac49c825c5d377e62ea7083b1"
                         "5a3d8adf3b57a527f89ec905aa00ed19", ciphertext);

    std::array<uint8_t, Crypto::kAesBlockSize> block;
    std::array<uint8_t, Crypto::kAesBlockSize * 3> plaintext;
    std::size_t outLen = 0;

    AESDecryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV()));

    std::array<uint8_t, Crypto::kAesBlockSize> in1, in2, in3;
    std::copy(ciphertext.begin(),                          ciphertext.begin() + 16, in1.begin());
    std::copy(ciphertext.begin() + 16,                     ciphertext.begin() + 32, in2.begin());
    std::copy(ciphertext.begin() + 32,                     ciphertext.begin() + 48, in3.begin());

    ASSERT_TRUE(ctx.update(in1, block, outLen));
    std::copy(block.begin(), block.begin() + outLen, plaintext.begin());
    ASSERT_TRUE(ctx.update(in2, block, outLen));
    std::copy(block.begin(), block.begin() + outLen, plaintext.begin() + 16);
    ASSERT_TRUE(ctx.update(in3, block, outLen));
    std::copy(block.begin(), block.begin() + outLen, plaintext.begin() + 32);

    ASSERT_TRUE(ctx.finish());

    std::array<uint8_t, Crypto::kAesBlockSize * 3> expected;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f"
                         "101112131415161718191a1b1c1d1e1f"
                         "202122232425262728292a2b2c2d2e2f", expected);

    ASSERT_EQ(plaintext, expected);
}

TEST(AESDecryptContext, NoPadding_InitTwiceFails)
{
    AESDecryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV()));
    ASSERT_FALSE(ctx.init(testKey(), testIV()));
}

TEST(AESDecryptContext, NoPadding_FinishWithoutInitFails)
{
    AESDecryptContext ctx;
    ASSERT_FALSE(ctx.finish());
}

// --- AESDecryptContext: PKCS#7 ---

TEST(AESDecryptContext, PKCS7_OneByte)
{
    std::array<uint8_t, Crypto::kAesBlockSize> ciphertext;
    TestUtil::hexToArray("94c9f8cffbf78d72697a13147d26bcfc", ciphertext);

    std::array<uint8_t, Crypto::kAesBlockSize * 2> outBuf;
    std::size_t updateLen = 0, finishLen = 0;

    AESDecryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(ciphertext, outBuf, updateLen));
    ASSERT_EQ(updateLen, 0u);
    ASSERT_TRUE(ctx.finish(outBuf, finishLen));
    ASSERT_EQ(finishLen, 1u);
    ASSERT_EQ(outBuf[0], uint8_t(0xAB));
}

TEST(AESDecryptContext, PKCS7_ExactBlock)
{
    // 16-byte plaintext produces 2 cipher blocks (data + padding).
    // update(ct1) → 0 bytes held back
    // update(ct2) → 16 bytes (plaintext)
    // finish()    → 0 bytes (full padding block stripped)
    // Note: PSA always physically writes one block to the finish output buffer
    // to prevent timing-based padding oracle attacks, so finish must use a
    // separate buffer to avoid overwriting the plaintext written by update().
    std::array<uint8_t, Crypto::kAesBlockSize * 2> ciphertext;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6"
                         "9de10d7592d73fe22aab9f56daa0856c", ciphertext);

    std::array<uint8_t, Crypto::kAesBlockSize> ct1, ct2;
    std::copy(ciphertext.begin(),      ciphertext.begin() + 16, ct1.begin());
    std::copy(ciphertext.begin() + 16, ciphertext.end(),        ct2.begin());

    std::array<uint8_t, Crypto::kAesBlockSize * 2> update1Buf, update2Buf, finishBuf;
    std::size_t len1 = 0, len2 = 0, finishLen = 0;

    AESDecryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(ct1, update1Buf, len1));
    ASSERT_EQ(len1, 0u);
    ASSERT_TRUE(ctx.update(ct2, update2Buf, len2));
    ASSERT_EQ(len2, Crypto::kAesBlockSize);
    ASSERT_TRUE(ctx.finish(finishBuf, finishLen));
    ASSERT_EQ(finishLen, 0u);

    std::array<uint8_t, Crypto::kAesBlockSize> expected;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", expected);

    ASSERT_EQ(DataRef(update2Buf.begin(), update2Buf.begin() + len2),
              DataRef(expected.begin(),   expected.end()));
}

TEST(AESDecryptContext, PKCS7_ThirtyTwoBytes)
{
    std::array<uint8_t, Crypto::kAesBlockSize * 3> ciphertext;
    TestUtil::hexToArray("d45d7c5aa26bca9d6b1d5ddbf68f9ef6"
                         "6dd101fac49c825c5d377e62ea7083b1"
                         "b3c3557de78fb44b7dcedd7344dbeaca", ciphertext);

    std::array<uint8_t, Crypto::kAesBlockSize> ct1, ct2, ct3;
    std::copy(ciphertext.begin(),      ciphertext.begin() + 16, ct1.begin());
    std::copy(ciphertext.begin() + 16, ciphertext.begin() + 32, ct2.begin());
    std::copy(ciphertext.begin() + 32, ciphertext.end(),        ct3.begin());

    std::array<uint8_t, Crypto::kAesBlockSize * 2> update1Buf, update2Buf, update3Buf, finishBuf;
    std::size_t len1 = 0, len2 = 0, len3 = 0, finishLen = 0;

    AESDecryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(ct1, update1Buf, len1));
    ASSERT_EQ(len1, 0u);
    ASSERT_TRUE(ctx.update(ct2, update2Buf, len2));
    ASSERT_EQ(len2, Crypto::kAesBlockSize);
    ASSERT_TRUE(ctx.update(ct3, update3Buf, len3));
    ASSERT_EQ(len3, Crypto::kAesBlockSize);
    ASSERT_TRUE(ctx.finish(finishBuf, finishLen));
    ASSERT_EQ(finishLen, 0u);

    std::array<uint8_t, Crypto::kAesBlockSize> expected1, expected2;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", expected1);
    TestUtil::hexToArray("101112131415161718191a1b1c1d1e1f", expected2);

    ASSERT_EQ(DataRef(update2Buf.begin(), update2Buf.begin() + len2),
              DataRef(expected1.begin(),  expected1.end()));
    ASSERT_EQ(DataRef(update3Buf.begin(), update3Buf.begin() + len3),
              DataRef(expected2.begin(),  expected2.end()));
}

TEST(AESDecryptContext, PKCS7_BadPaddingFails)
{
    // Ciphertext with a deliberately corrupted padding byte.
    std::array<uint8_t, Crypto::kAesBlockSize> ciphertext;
    TestUtil::hexToArray("94c9f8cffbf78d72697a13147d26bc03", ciphertext);

    std::array<uint8_t, Crypto::kAesBlockSize * 2> outBuf;
    std::size_t updateLen = 0, finishLen = 0;

    AESDecryptContext ctx;
    ASSERT_TRUE(ctx.init(testKey(), testIV(), true));
    ASSERT_TRUE(ctx.update(ciphertext, outBuf, updateLen));
    ASSERT_FALSE(ctx.finish(outBuf, finishLen));
}

// --- Round-trip ---

TEST(AESContext, RoundTrip_PKCS7_OneByte)
{
    const std::array<uint8_t, 1> plaintext = { 0x42 };

    std::array<uint8_t, Crypto::kAesBlockSize * 2> encrypted;
    std::size_t encryptedLen = 0;

    {
        std::array<uint8_t, Crypto::kAesBlockSize> unused;
        std::size_t unusedLen = 0;

        std::array<uint8_t, Crypto::kAesBlockSize * 2> finishBuf;
        std::size_t finishLen = 0;

        std::array<uint8_t, Crypto::kAesBlockSize> partialIn = {};
        partialIn[0] = plaintext[0];

        AESEncryptContext enc;
        ASSERT_TRUE(enc.init(testKey(), testIV(), true));
        ASSERT_TRUE(enc.update(partialIn, 1, unused, unusedLen));
        ASSERT_TRUE(enc.finish(finishBuf, finishLen));

        std::copy(finishBuf.begin(), finishBuf.begin() + finishLen, encrypted.begin());
        encryptedLen = finishLen;
    }

    std::array<uint8_t, Crypto::kAesBlockSize> cipherBlock;
    std::copy(encrypted.begin(), encrypted.begin() + encryptedLen, cipherBlock.begin());

    std::array<uint8_t, Crypto::kAesBlockSize * 2> decryptBuf;
    std::size_t updateLen = 0, finishLen = 0;

    AESDecryptContext dec;
    ASSERT_TRUE(dec.init(testKey(), testIV(), true));
    ASSERT_TRUE(dec.update(cipherBlock, decryptBuf, updateLen));
    ASSERT_EQ(updateLen, 0u);
    ASSERT_TRUE(dec.finish(decryptBuf, finishLen));
    ASSERT_EQ(finishLen, 1u);
    ASSERT_EQ(decryptBuf[0], plaintext[0]);
}

TEST(AESContext, RoundTrip_PKCS7_ExactBlock)
{
    std::array<uint8_t, Crypto::kAesBlockSize> plaintext;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f", plaintext);

    // Encrypt
    std::array<uint8_t, Crypto::kAesBlockSize> encUpdate;
    std::array<uint8_t, Crypto::kAesBlockSize * 2> encFinish;
    std::size_t encUpdateLen = 0, encFinishLen = 0;

    AESEncryptContext enc;
    ASSERT_TRUE(enc.init(testKey(), testIV(), true));
    ASSERT_TRUE(enc.update(plaintext, encUpdate, encUpdateLen));
    ASSERT_TRUE(enc.finish(encFinish, encFinishLen));

    // Decrypt - use separate buffers for each call to avoid PSA overwriting
    // valid plaintext (PSA always physically writes one block to the finish
    // output regardless of the stripped length, to prevent timing attacks).
    std::array<uint8_t, Crypto::kAesBlockSize> ct1, ct2;
    std::copy(encUpdate.begin(), encUpdate.begin() + encUpdateLen, ct1.begin());
    std::copy(encFinish.begin(), encFinish.begin() + encFinishLen, ct2.begin());

    std::array<uint8_t, Crypto::kAesBlockSize * 2> dec1Buf, dec2Buf, decFinishBuf;
    std::size_t decLen1 = 0, decLen2 = 0, decFinish = 0;

    AESDecryptContext dec;
    ASSERT_TRUE(dec.init(testKey(), testIV(), true));
    ASSERT_TRUE(dec.update(ct1, dec1Buf, decLen1));
    ASSERT_EQ(decLen1, 0u);
    ASSERT_TRUE(dec.update(ct2, dec2Buf, decLen2));
    ASSERT_EQ(decLen2, Crypto::kAesBlockSize);
    ASSERT_TRUE(dec.finish(decFinishBuf, decFinish));
    ASSERT_EQ(decFinish, 0u);

    ASSERT_EQ(DataRef(dec2Buf.begin(), dec2Buf.begin() + decLen2),
              DataRef(plaintext.begin(), plaintext.end()));
}

TEST(AESContext, RoundTrip_PKCS7_ThirtyTwoBytes)
{
    std::array<uint8_t, Crypto::kAesBlockSize * 2> plaintext;
    TestUtil::hexToArray("000102030405060708090a0b0c0d0e0f"
                         "101112131415161718191a1b1c1d1e1f", plaintext);

    std::array<uint8_t, Crypto::kAesBlockSize> pt1, pt2;
    std::copy(plaintext.begin(),      plaintext.begin() + 16, pt1.begin());
    std::copy(plaintext.begin() + 16, plaintext.end(),        pt2.begin());

    // Encrypt
    std::array<uint8_t, Crypto::kAesBlockSize> enc1, enc2;
    std::array<uint8_t, Crypto::kAesBlockSize * 2> encFinish;
    std::size_t encLen1 = 0, encLen2 = 0, encFinishLen = 0;

    AESEncryptContext enc;
    ASSERT_TRUE(enc.init(testKey(), testIV(), true));
    ASSERT_TRUE(enc.update(pt1, enc1, encLen1));
    ASSERT_TRUE(enc.update(pt2, enc2, encLen2));
    ASSERT_TRUE(enc.finish(encFinish, encFinishLen));

    // Decrypt - use separate buffers for each call (see PKCS7_ExactBlock comment)
    std::array<uint8_t, Crypto::kAesBlockSize> ct3;
    std::copy(encFinish.begin(), encFinish.begin() + encFinishLen, ct3.begin());

    std::array<uint8_t, Crypto::kAesBlockSize * 2> dec1Buf, dec2Buf, dec3Buf, decFinishBuf;
    std::size_t decLen1 = 0, decLen2 = 0, decLen3 = 0, decFinish = 0;

    AESDecryptContext dec;
    ASSERT_TRUE(dec.init(testKey(), testIV(), true));
    ASSERT_TRUE(dec.update(enc1, dec1Buf, decLen1));
    ASSERT_EQ(decLen1, 0u);
    ASSERT_TRUE(dec.update(enc2, dec2Buf, decLen2));
    ASSERT_EQ(decLen2, Crypto::kAesBlockSize);
    ASSERT_TRUE(dec.update(ct3, dec3Buf, decLen3));
    ASSERT_EQ(decLen3, Crypto::kAesBlockSize);
    ASSERT_TRUE(dec.finish(decFinishBuf, decFinish));
    ASSERT_EQ(decFinish, 0u);

    ASSERT_EQ(DataRef(dec2Buf.begin(), dec2Buf.begin() + decLen2),
              DataRef(pt1.begin(), pt1.end()));
    ASSERT_EQ(DataRef(dec3Buf.begin(), dec3Buf.begin() + decLen3),
              DataRef(pt2.begin(), pt2.end()));
}
