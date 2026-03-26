#ifndef INCLUDED_HMACCONTEXT_H
#define INCLUDED_HMACCONTEXT_H

#include "crypto/cryptotypes.h"
#include "types/dataref.h"

#include <psa/crypto.h>

class HMACContext
{
public:
    static bool generate(const Crypto::Key& key,
                         const DataRef& data,
                         Crypto::HMAC& hmac);

private:
    enum class State
    {
        kInternalError = -1,
        kUninitialized = 0,
        kInitialized   = 1,
        kFinished      = 2
    };
    
public:
    HMACContext();
    ~HMACContext();
    
public:
    bool init(const Crypto::Key& key);
    bool update(const DataRef& data);
    bool finish(Crypto::HMAC& hmac);
    
private:
    State                 mState;
    psa_mac_operation_t   mOperation;
    mbedtls_svc_key_id_t  mKeyId;
    
private:
    HMACContext(const HMACContext&) = delete;
    HMACContext& operator=(const HMACContext&) = delete;
};

#endif
