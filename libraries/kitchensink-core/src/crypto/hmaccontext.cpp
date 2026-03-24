#include "crypto/hmaccontext.h"

Crypto::HMAC HMACContext::generate(const Crypto::Key& key,
                                   const DataRef&     data)
{
    HMACContext hmac;

    if (!hmac.init(key) || !hmac.update(data))
    {
        Crypto::HMAC failed;
        failed.fill(0);
        return failed;
    }
    
    return hmac.finish();
}

HMACContext::HMACContext()
    : mContextInitialized(false)
{ }

HMACContext::~HMACContext()
{
    if (mContextInitialized)
    {
        mbedtls_md_free(&mContext);
    }
}

bool HMACContext::init(const Crypto::Key& key)
{
    if (mContextInitialized)
    {
        return false;
    }
    
    mbedtls_md_init(&mContext);
    
    int setupRc = mbedtls_md_setup(&mContext,
                                   mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                                   1);

    if (setupRc != 0)
    {
        return false;
    }
    
    int startsRc = mbedtls_md_hmac_starts(&mContext, key.begin(), key.size());

    if (startsRc != 0)
    {
        mbedtls_md_free(&mContext);
        return false;
    }

    mContextInitialized = true;
    return true;
}

bool HMACContext::update(const DataRef& data)
{
    int updateRc = mbedtls_md_hmac_update(&mContext, data.begin(), data.size());

    return updateRc == 0;
}

Crypto::HMAC HMACContext::finish()
{
    Crypto::HMAC hmac;
    hmac.fill(0);

    int finishRc = mbedtls_md_hmac_finish(&mContext, hmac.begin());
    
    mbedtls_md_free(&mContext);

    mContextInitialized = false;

    if (finishRc != 0)
    {
        hmac.fill(0);
    }
        
    return hmac;
}

