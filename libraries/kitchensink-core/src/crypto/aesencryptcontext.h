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
    AESEncryptContext(const AESEncryptContext&) = delete;
    AESEncryptContext& operator=(const AESEncryptContext&) = delete;
};

template <std::size_t Capacity>
inline
bool AESEncryptContext::update(const std::array<uint8_t, Capacity>& in,
                               std::array<uint8_t, Capacity>&       out)
{
    return update(in.begin(), out.begin(), Capacity);
}

#endif
