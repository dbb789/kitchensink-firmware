#ifndef INCLUDED_AESDECRYPTCONTEXT_H
#define INCLUDED_AESDECRYPTCONTEXT_H

#include "crypto/cryptotypes.h"

#include <PSA_Crypto.h>

#include <array>
#include <cstddef>

class AESDecryptContext
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
    AESDecryptContext();
    ~AESDecryptContext();

public:
    bool init(const Crypto::Key& key,
              const Crypto::IV&  iv,
              bool               pkcs7 = false);
    
    template <std::size_t Capacity>
    bool update(const std::array<uint8_t, Capacity>& in,
                std::array<uint8_t, Capacity>&       out);

    template <std::size_t Capacity, std::size_t OutCapacity>
    bool update(const std::array<uint8_t, Capacity>& in,
                std::array<uint8_t, OutCapacity>&    out,
                std::size_t&                         outLen);

    template <std::size_t Capacity>
    bool finish(std::array<uint8_t, Capacity>& out,
                std::size_t&                   outLen);
    
    bool finish();

private:
    bool update(const uint8_t* in,
                std::size_t    inLen,
                uint8_t*       out,
                std::size_t    outBufLen,
                std::size_t&   outLen);
    
    bool finish(uint8_t*     out,
                std::size_t  maxOut,
                std::size_t& outLen);
private:
    State                  mState;
    psa_cipher_operation_t mOperation;
    mbedtls_svc_key_id_t   mKeyId;

private:
    AESDecryptContext(const AESDecryptContext&) = delete;
    AESDecryptContext& operator=(const AESDecryptContext&) = delete;
};

template <std::size_t Capacity>
inline
bool AESDecryptContext::update(const std::array<uint8_t, Capacity>& in,
                               std::array<uint8_t, Capacity>&       out)
{
    std::size_t outLen = 0;
    
    return update<Capacity, Capacity>(in, out, outLen);
}

template <std::size_t Capacity, std::size_t OutCapacity>
inline
bool AESDecryptContext::update(const std::array<uint8_t, Capacity>& in,
                               std::array<uint8_t, OutCapacity>&    out,
                               std::size_t&                         outLen)
{
    static_assert(OutCapacity >= Capacity, "Output buffer must be at least as large as input buffer.");
    
    return update(in.begin(),
                  Capacity,
                  out.begin(),
                  OutCapacity,
                  outLen);
}

template <std::size_t Capacity>
inline
bool AESDecryptContext::finish(std::array<uint8_t, Capacity>& out,
                               std::size_t&                   outLen)
{
    return finish(out.begin(), Capacity, outLen);
}

#endif
