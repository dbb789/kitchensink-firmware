#include "crypto/hmaccontext.h"

#include <cassert>

Crypto::HMAC HMACContext::generate(const Crypto::Key& key,
                                   const DataRef&     data)
{
    HMACContext hmac;

    hmac.init(key);
    hmac.update(data);
    
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

void HMACContext::init(const Crypto::Key& key)
{
    assert(!mContextInitialized);
    
    mbedtls_md_init(&mContext);
    
    int setupRc = mbedtls_md_setup(&mContext,
                                   mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                                   1);

    assert(setupRc == 0);
    
    int startsRc = mbedtls_md_hmac_starts(&mContext, key.begin(), key.size());

    assert(startsRc == 0);

    mContextInitialized = true;
}

void HMACContext::update(const DataRef& data)
{
    int updateRc = mbedtls_md_hmac_update(&mContext, data.begin(), data.size());

    assert(updateRc == 0);
}

Crypto::HMAC HMACContext::finish()
{
    Crypto::HMAC hmac;

    int finishRc = mbedtls_md_hmac_finish(&mContext, hmac.begin());

    assert(finishRc == 0);
    
    mbedtls_md_free(&mContext);

    mContextInitialized = false;
        
    return hmac;
}

