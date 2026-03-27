#include "crypto/cryptoutil.h"

#include "types/dataref.h"
#include "types/strbuf.h"
#include "config.h"

#include <PSA_Crypto.h>

#include <cstdint>
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

bool pbkdf2HmacSha512(const StrRef&     password,
                      const StrRef&     suffix,
                      const Crypto::IV& salt,
                      uint32_t          iterations,
                      Crypto::Key&      key)
{
    initializeLibrary();

    key.fill(0);

    // Concatenate password + suffix as raw bytes (UTF-8/ASCII).
    if (password.length() + suffix.length() > Config::kPasswordMax)
    {
        return false;
    }

    std::array<uint8_t, Config::kPasswordMax> pwdBytes;
    std::size_t pwdLen(0);

    for (std::size_t i = 0; i < password.length(); ++i)
    {
        pwdBytes[pwdLen++] = uint8_t(password[i]);
    }

    for (std::size_t i = 0; i < suffix.length(); ++i)
    {
        pwdBytes[pwdLen++] = uint8_t(suffix[i]);
    }

    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;

    if (psa_key_derivation_setup(&op, PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_512)) != PSA_SUCCESS)
    {
        return false;
    }

    if (psa_key_derivation_input_integer(&op, PSA_KEY_DERIVATION_INPUT_COST, iterations) != PSA_SUCCESS)
    {
        psa_key_derivation_abort(&op);
        return false;
    }

    if (psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_SALT, salt.data(), salt.size()) != PSA_SUCCESS)
    {
        psa_key_derivation_abort(&op);
        return false;
    }

    if (psa_key_derivation_input_bytes(&op,
                                       PSA_KEY_DERIVATION_INPUT_PASSWORD,
                                       pwdBytes.data(),
                                       pwdLen) != PSA_SUCCESS)
    {
        psa_key_derivation_abort(&op);
        return false;
    }

    if (psa_key_derivation_output_bytes(&op, key.data(), key.size()) != PSA_SUCCESS)
    {
        psa_key_derivation_abort(&op);
        key.fill(0);
        return false;
    }

    psa_key_derivation_abort(&op);

    return true;
}

}
