#ifndef INCLUDED_CRYPTOOUTSTREAM_H
#define INCLUDED_CRYPTOOUTSTREAM_H

#include "crypto/aesencryptcontext.h"
#include "crypto/cryptoutil.h"
#include "crypto/cryptotypes.h"
#include "crypto/hmaccontext.h"
#include "config.h"
#include "types/circularstream.h"
#include "types/outstream.h"
#include "types/strbuf.h"
#include "types/stroutstream.h"
#include "types/arrayoutstream.h"

class DataRef;

class CryptoOutStream : public OutStream
{
public:
    enum class State
    {
        kInternalError = -1,
        kWriting       = 0,
        kFlushed       = 3
    };

public:
    CryptoOutStream(OutStream&         outStream,
                    const StrRef&      password,
                    const StrRef&      suffix,
                    const Crypto::IV&  iv,
                    const Crypto::IV&  dataIv,
                    const Crypto::Key& dataKey,
                    uint32_t           kdfIterations = Config::kKdfIterations);

public:
    virtual ~CryptoOutStream();
        
public:
    virtual std::size_t write(const DataRef& data) override;

    State state() const;

private:
    void writeHeader(const Crypto::IV& dataIv, const Crypto::Key& dataKey, uint32_t kdfIterations);
    void flush();

private:
    OutStream&                                 mOutStream;
    StrRef                                     mPassword;
    StrRef                                     mSuffix;
    Crypto::IV                                 mIv;
    AESEncryptContext                          mEncryptContext;
    HMACContext                                mHMAC;
    std::array<uint8_t, Crypto::kAesBlockSize> mData;
    ArrayOutStream                             mDataOut;
    State                                      mState;
        
private:
    friend class SecureStorage;
};

inline
CryptoOutStream::State CryptoOutStream::state() const
{
    return mState;
}

#endif










