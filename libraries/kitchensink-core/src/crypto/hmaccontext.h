#ifndef INCLUDED_HMACCONTEXT_H
#define INCLUDED_HMACCONTEXT_H

#include "crypto/cryptotypes.h"
#include "types/dataref.h"

#include <mbedtls/md.h>

class HMACContext
{
public:
    static Crypto::HMAC generate(const Crypto::Key& key,
                                 const DataRef& data);
    
public:
    HMACContext();
    ~HMACContext();
    
public:
    bool init(const Crypto::Key& key);
    bool update(const DataRef& data);
    Crypto::HMAC finish();
    
private:
    bool                 mContextInitialized;
    mbedtls_md_context_t mContext;
    
private:
    HMACContext(const HMACContext&) = delete;
    HMACContext& operator=(const HMACContext&) = delete;
};

#endif
