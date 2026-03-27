#ifndef INCLUDED_AESENCRYPTCONTEXT_H
#define INCLUDED_AESENCRYPTCONTEXT_H

#include "crypto/cryptotypes.h"

#include <PSA_Crypto.h>

#include <array>
#include <cstddef>

class AESEncryptContext
{
private:
    enum class State
    {
        kInternalError = -1,
        kUninitialized =  0,
        kInitialized   =  1,
        kFinished      =  2
    };

public:
    AESEncryptContext();
    ~AESEncryptContext();

public:
    bool init(const Crypto::Key& key,
              const Crypto::IV&  iv,
              bool               pkcs7 = false);
    bool update(const uint8_t* in,
                std::size_t    inLen,
                uint8_t*       out,
                std::size_t    outBufLen,
                std::size_t&   outLen);
    bool finish();
    bool finish(uint8_t*     out,
                std::size_t  maxOut,
                std::size_t& outLen);

public:
    template <std::size_t Capacity>
    bool update(const std::array<uint8_t, Capacity>& in,
                std::array<uint8_t, Capacity>&       out);

    template <std::size_t Capacity>
    bool finish(std::array<uint8_t, Capacity>& out,
                std::size_t&                   outLen);

private:
    State                  mState;
    psa_cipher_operation_t mOperation;
    mbedtls_svc_key_id_t   mKeyId;

private:
    AESEncryptContext(const AESEncryptContext&) = delete;
    AESEncryptContext& operator=(const AESEncryptContext&) = delete;
};

template <std::size_t Capacity>
inline
bool AESEncryptContext::update(const std::array<uint8_t, Capacity>& in,
                               std::array<uint8_t, Capacity>&       out)
{
    std::size_t outLen = 0;
    return update(in.begin(), Capacity, out.begin(), Capacity, outLen);
}

template <std::size_t Capacity>
inline
bool AESEncryptContext::finish(std::array<uint8_t, Capacity>& out,
                               std::size_t&                   outLen)
{
    return finish(out.begin(), Capacity, outLen);
}

#endif
