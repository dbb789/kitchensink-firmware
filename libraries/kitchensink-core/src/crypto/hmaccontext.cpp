#include "crypto/hmaccontext.h"

#include "crypto/cryptoutil.h"

bool HMACContext::generate(const Crypto::Key& key,
                           const DataRef&     data,
                           Crypto::HMAC&      hmac)
{
    HMACContext context;

    if (!context.init(key) || !context.update(data))
    {
        return false;
    }
    
    hmac = context.finish();
    
    return true;
}

HMACContext::HMACContext()
    : mContextInitialized(false)
    , mOperation(PSA_MAC_OPERATION_INIT)
    , mKeyId(MBEDTLS_SVC_KEY_ID_INIT)
{ }

HMACContext::~HMACContext()
{
    if (mContextInitialized)
    {
        psa_mac_abort(&mOperation);
        psa_destroy_key(mKeyId);
    }
}

bool HMACContext::init(const Crypto::Key& key)
{
    if (mContextInitialized)
    {
        return false;
    }

    CryptoUtil::initializeLibrary();
    
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(key.size()));

    if (psa_import_key(&attributes, key.data(), key.size(), &mKeyId) != PSA_SUCCESS)
    {
        return false;
    }

    if (psa_mac_sign_setup(&mOperation, mKeyId, PSA_ALG_HMAC(PSA_ALG_SHA_256)) != PSA_SUCCESS)
    {
        psa_destroy_key(mKeyId);
        mKeyId = MBEDTLS_SVC_KEY_ID_INIT;
        return false;
    }

    mContextInitialized = true;
    return true;
}

bool HMACContext::update(const DataRef& data)
{
    return psa_mac_update(&mOperation, data.begin(), data.size()) == PSA_SUCCESS;
}

Crypto::HMAC HMACContext::finish()
{
    Crypto::HMAC hmac;
    hmac.fill(0);

    size_t macLength = 0;
    psa_status_t status = psa_mac_sign_finish(&mOperation,
                                              hmac.data(),
                                              hmac.size(),
                                              &macLength);

    psa_mac_abort(&mOperation);
    psa_destroy_key(mKeyId);
    mKeyId = MBEDTLS_SVC_KEY_ID_INIT;
    mContextInitialized = false;

    if (status != PSA_SUCCESS)
    {
        hmac.fill(0);
    }
        
    return hmac;
}

