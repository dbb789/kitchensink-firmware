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

bool pbkdf2HmacSha512(const StrRef&     password,
                      const StrRef&     suffix,
                      const Crypto::IV& salt,
                      uint32_t          iterations,
                      Crypto::Key&      key);

template <std::size_t Capacity>
inline
bool sha256(const std::array<uint8_t, Capacity>& data,
            Crypto::SHA256&                      hash)
{
    return sha256(data.begin(),
                  data.end(),
                  hash);
}

}

#endif
