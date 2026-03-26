#ifndef INCLUDED_CRYPTOOUTSTREAM_H
#define INCLUDED_CRYPTOOUTSTREAM_H

#include "crypto/cryptoutil.h"
#include "crypto/cryptotypes.h"
#include "crypto/hmaccontext.h"
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
                    const Crypto::IV&  iv,
                    const Crypto::IV&  dataIv,
                    const Crypto::Key& dataKey);

public:
    virtual ~CryptoOutStream();
        
public:
    virtual std::size_t write(const DataRef& data) override;

    State state() const;

private:
    void writeHeader();
    void flush();

private:
    OutStream&                                 mOutStream;
    StrRef                                     mPassword;
    Crypto::IV                                 mIv;
    Crypto::IV                                 mDataIv;
    Crypto::Key                                mDataKey;
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










