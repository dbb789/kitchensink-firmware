#include "crypto/cryptoutil.h"

#include "types/dataref.h"
#include "types/strbuf.h"
#include "config.h"

#define MBEDTLS_ALLOW_PRIVATE_ACCESS
#include <mbedtls/private/aes.h>
#include <mbedtls/private/sha256.h>
#undef MBEDTLS_ALLOW_PRIVATE_ACCESS

#include <psa/crypto.h>

#include <mbedtls/md.h>

namespace CryptoUtil
{

namespace
{

bool psaCryptoInitialized = false;

}

void initializeLibrary()
{
    if (!psaCryptoInitialized)
    {
        if (psa_crypto_init() != PSA_SUCCESS)
        {
            throw std::runtime_error("Failed to initialize PSA Crypto library");
        }
        
        psaCryptoInitialized = true;
    }
}

bool sha256(const uint8_t*  begin,
            const uint8_t*  end,
            Crypto::SHA256& hash)
{
    size_t hashLength(0);
    
    if (psa_hash_compute(PSA_ALG_SHA_256,
                         begin,
                         end - begin,
                         hash.data(),
                         hash.size(),
                         &hashLength) != PSA_SUCCESS)
    {
        hash.fill(0);
        
        return false;
    }
    
    return true;
}

Crypto::Key stretch(const StrRef&     password,
                    const Crypto::IV& iv)
{
    Crypto::Key digest;

    digest.fill(0);

    std::copy(iv.begin(), iv.end(), digest.begin());
    
    // Dirty UTF-16LE conversion for 7-bit ASCII.
    std::array<uint8_t, (Config::kPasswordMax + Config::kPasswordSuffix.length()) * 2> pwdUtf16;
    std::size_t pwdUtf16Len(0);

    for (std::size_t i = 0; i < password.length() && i < Config::kPasswordMax; ++i)
    {
        pwdUtf16[pwdUtf16Len++] = password[i];
        pwdUtf16[pwdUtf16Len++] = 0;
    }
    
    // Also, concatenate the firmware password suffix onto the end.
    for (auto& c : Config::kPasswordSuffix)
    {
        pwdUtf16[pwdUtf16Len++] = c;
        pwdUtf16[pwdUtf16Len++] = 0;
    }

    // 8192 rounds of SHA256 on the IV and password as per AESCrypt.
    for (int i(0); i < 8192; ++i)
    {
        mbedtls_sha256_context ctx;
        
        mbedtls_sha256_init(&ctx);
        mbedtls_sha256_starts(&ctx, 0);
        mbedtls_sha256_update(&ctx, digest.begin(), digest.size());
        mbedtls_sha256_update(&ctx,
                              pwdUtf16.begin(),
                              pwdUtf16Len);
        mbedtls_sha256_finish(&ctx, digest.begin());
        mbedtls_sha256_free(&ctx);
    }

    return digest;
}

bool encrypt(const Crypto::Key& key,
             const Crypto::IV&  iv,
             std::size_t        size,
             const uint8_t*     source,
             uint8_t*           dest,
             Crypto::IV&        nextIv)
{
    nextIv = iv;
    mbedtls_aes_context ctx;
    
    mbedtls_aes_init(&ctx);
    
    int encRc = mbedtls_aes_setkey_enc(&ctx, key.begin(), 256);

    if (encRc != 0)
    {
        mbedtls_aes_free(&ctx);
        return false;
    }
    
    int cbcRc = mbedtls_aes_crypt_cbc(&ctx,
                                      MBEDTLS_AES_ENCRYPT,
                                      size,
                                      nextIv.begin(),
                                      source,
                                      dest);

    mbedtls_aes_free(&ctx);

    return cbcRc == 0;
}

bool decrypt(const Crypto::Key& key,
             const Crypto::IV&  iv,
             std::size_t        size,
             const uint8_t*     source,
             uint8_t*           dest,
             Crypto::IV&        nextIv)
{
    nextIv = iv;
    mbedtls_aes_context ctx;
    
    mbedtls_aes_init(&ctx);

    int decRc = mbedtls_aes_setkey_dec(&ctx, key.begin(), 256);

    if (decRc != 0)
    {
        mbedtls_aes_free(&ctx);
        return false;
    }
    
    int cbcRc = mbedtls_aes_crypt_cbc(&ctx,
                                      MBEDTLS_AES_DECRYPT,
                                      size,
                                      nextIv.begin(),
                                      source,
                                      dest);

    mbedtls_aes_free(&ctx);

    return cbcRc == 0;
}

}
