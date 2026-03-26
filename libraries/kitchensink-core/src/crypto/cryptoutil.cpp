#include "crypto/cryptoutil.h"

#include "types/dataref.h"
#include "types/strbuf.h"
#include "config.h"

#include <PSA_Crypto.h>

#include <string.h>

extern "C"
{
psa_status_t mbedtls_psa_external_get_random(mbedtls_psa_external_random_context_t *context,
                                             uint8_t *output, size_t output_size, size_t *output_length)
{
    return PSA_ERROR_NOT_SUPPORTED;
}
}

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
            std::terminate();
        }
        
        psaCryptoInitialized = true;
    }
}

bool sha256(const uint8_t*  begin,
            const uint8_t*  end,
            Crypto::SHA256& hash)
{
    initializeLibrary();
    
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

bool stretch(const StrRef&     password,
             const Crypto::IV& iv,
             Crypto::Key&      digest)
{
    initializeLibrary();
    
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
        psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;

        psa_hash_setup(&operation, PSA_ALG_SHA_256);

        psa_hash_update(&operation, digest.begin(), digest.size());
        psa_hash_update(&operation, pwdUtf16.begin(), pwdUtf16Len);

        size_t outputLen(0);
        
        psa_hash_finish(&operation, digest.begin(), digest.size(), &outputLen);
    }

    return true;
}

bool encrypt(const Crypto::Key& key,
             const Crypto::IV&  iv,
             std::size_t        size,
             const uint8_t*     source,
             uint8_t*           dest,
             Crypto::IV&        nextIv)
{
    initializeLibrary();

    mbedtls_svc_key_id_t keyId = MBEDTLS_SVC_KEY_ID_INIT;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(key.size()));

    if (psa_import_key(&attributes, key.data(), key.size(), &keyId) != PSA_SUCCESS)
    {
        return false;
    }

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

    if (psa_cipher_encrypt_setup(&operation, keyId, PSA_ALG_CBC_NO_PADDING) != PSA_SUCCESS)
    {
        psa_destroy_key(keyId);
        return false;
    }

    if (psa_cipher_set_iv(&operation, iv.data(), iv.size()) != PSA_SUCCESS)
    {
        psa_cipher_abort(&operation);
        psa_destroy_key(keyId);
        return false;
    }

    size_t outputLength = 0;

    if (psa_cipher_update(&operation, source, size, dest, size, &outputLength) != PSA_SUCCESS)
    {
        psa_cipher_abort(&operation);
        psa_destroy_key(keyId);
        return false;
    }

    size_t finishLength = 0;

    if (psa_cipher_finish(&operation, dest + outputLength, size - outputLength, &finishLength) != PSA_SUCCESS)
    {
        psa_cipher_abort(&operation);
        psa_destroy_key(keyId);
        return false;
    }

    psa_destroy_key(keyId);

    // Update nextIv to the last ciphertext block for CBC chaining.
    std::copy(dest + size - Crypto::kAesBlockSize, dest + size, nextIv.begin());

    return true;
}

bool decrypt(const Crypto::Key& key,
             const Crypto::IV&  iv,
             std::size_t        size,
             const uint8_t*     source,
             uint8_t*           dest,
             Crypto::IV&        nextIv)
{
    initializeLibrary();

    mbedtls_svc_key_id_t keyId = MBEDTLS_SVC_KEY_ID_INIT;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(key.size()));

    if (psa_import_key(&attributes, key.data(), key.size(), &keyId) != PSA_SUCCESS)
    {
        return false;
    }

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

    if (psa_cipher_decrypt_setup(&operation, keyId, PSA_ALG_CBC_NO_PADDING) != PSA_SUCCESS)
    {
        psa_destroy_key(keyId);
        return false;
    }

    if (psa_cipher_set_iv(&operation, iv.data(), iv.size()) != PSA_SUCCESS)
    {
        psa_cipher_abort(&operation);
        psa_destroy_key(keyId);
        return false;
    }

    size_t outputLength = 0;

    if (psa_cipher_update(&operation, source, size, dest, size, &outputLength) != PSA_SUCCESS)
    {
        psa_cipher_abort(&operation);
        psa_destroy_key(keyId);
        return false;
    }

    size_t finishLength = 0;

    if (psa_cipher_finish(&operation, dest + outputLength, size - outputLength, &finishLength) != PSA_SUCCESS)
    {
        psa_cipher_abort(&operation);
        psa_destroy_key(keyId);
        return false;
    }

    psa_destroy_key(keyId);

    // Update nextIv to the last ciphertext block for CBC chaining.
    std::copy(source + size - Crypto::kAesBlockSize, source + size, nextIv.begin());

    return true;
}

}
