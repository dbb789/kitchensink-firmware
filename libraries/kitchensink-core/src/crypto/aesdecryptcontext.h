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
    bool init(const Crypto::Key& key, const Crypto::IV& iv);
    bool update(const uint8_t* in, uint8_t* out, std::size_t size);
    bool finish();

public:
    template <std::size_t Capacity>
    bool update(const std::array<uint8_t, Capacity>& in,
                std::array<uint8_t, Capacity>&       out);

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
    return update(in.begin(), out.begin(), Capacity);
}

#endif
