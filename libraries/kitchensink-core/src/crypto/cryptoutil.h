#ifndef INCLUDED_CRYPTOUTIL_H
#define INCLUDED_CRYPTOUTIL_H

#include "crypto/cryptotypes.h"

#include <array>
#include <cstdint>

class DataRef;
class StrRef;

namespace CryptoUtil
{

void initializeLibrary();

bool sha256(const uint8_t*  begin,
            const uint8_t*  end,
            Crypto::SHA256& hash);

bool stretch(const StrRef&     password,
             const Crypto::IV& iv,
             Crypto::Key&      digest);

bool encrypt(const Crypto::Key& key,
             const Crypto::IV&  iv,
             std::size_t        size,
             const uint8_t*     source,
             uint8_t*           dest,
             Crypto::IV&        nextIv);

bool decrypt(const Crypto::Key& key,
             const Crypto::IV&  iv,
             std::size_t        size,
             const uint8_t*     source,
             uint8_t*           dest,
             Crypto::IV&        nextIv);


template <std::size_t Capacity>
inline
bool sha256(const std::array<uint8_t, Capacity>& data,
            Crypto::SHA256&                      hash)
{
    return sha256(data.begin(),
                  data.end(),
                  hash);
}

template <std::size_t Capacity>
inline
bool encrypt(const Crypto::Key&                   key,
             const Crypto::IV&                    iv,
             const std::array<uint8_t, Capacity>& source,
             std::array<uint8_t, Capacity>&       dest,
             Crypto::IV&                          nextIv)
{
    return encrypt(key,
                   iv,
                   Capacity,
                   source.begin(),
                   dest.begin(),
                   nextIv);
}

template <std::size_t Capacity>
inline
bool decrypt(const Crypto::Key&                   key,
             const Crypto::IV&                    iv,
             const std::array<uint8_t, Capacity>& source,
             std::array<uint8_t, Capacity>&       dest,
             Crypto::IV&                          nextIv)
{
    return decrypt(key,
                   iv,
                   Capacity,
                   source.begin(),
                   dest.begin(),
                   nextIv);
}

}

#endif
