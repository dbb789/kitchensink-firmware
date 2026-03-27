#include "crypto/cryptooutstream.h"

#include "crypto/aesencryptcontext.h"
#include "types/arrayutil.h"
#include "types/stroutstream.h"
#include "config.h"

CryptoOutStream::CryptoOutStream(OutStream&         outStream,
                                 const StrRef&      password,
                                 const StrRef&      suffix,
                                 const Crypto::IV&  iv,
                                 const Crypto::IV&  dataIv,
                                 const Crypto::Key& dataKey,
                                 uint32_t           kdfIterations)
    : mOutStream(outStream)
    , mPassword(password)
    , mSuffix(suffix)
    , mIv(iv)
    , mData()
    , mDataOut(mData)
    , mState(State::kWriting)
{
    writeHeader(dataIv, dataKey, kdfIterations);
}

CryptoOutStream::~CryptoOutStream()
{
    flush();
}

void CryptoOutStream::writeHeader(const Crypto::IV&  dataIv,
                                  const Crypto::Key& dataKey,
                                  uint32_t           kdfIterations)
{
    Crypto::Key key;
    
    if (!CryptoUtil::pbkdf2HmacSha512(mPassword, mSuffix, mIv, kdfIterations, key))
    {
        mState = State::kInternalError;
        return;
    }

    std::array<uint8_t, sizeof(dataIv) + sizeof(dataKey)> dataIvKey;

    ArrayUtil<decltype(dataIvKey)>::join(dataIv, dataKey, dataIvKey);
    
    std::array<uint8_t, sizeof(dataIv) + sizeof(dataKey)> dataIvKeyCrypt;

    {
        AESEncryptContext headerEncrypt;

        if (!headerEncrypt.init(key, mIv) ||
            !headerEncrypt.update(dataIvKey, dataIvKeyCrypt) ||
            !headerEncrypt.finish())
        {
            mState = State::kInternalError;
            return;
        }
    }

    // HMAC input is the encrypted IV+Key followed by the version byte 0x03.
    std::array<uint8_t, sizeof(dataIv) + sizeof(dataKey) + 1> dataIvKeyWithVersion;
    std::copy(dataIvKeyCrypt.begin(), dataIvKeyCrypt.end(), dataIvKeyWithVersion.begin());
    dataIvKeyWithVersion[sizeof(dataIv) + sizeof(dataKey)] = 0x03;

    Crypto::HMAC dataIvKeyHmac;
    
    if (!HMACContext::generate(key,
                               DataRef(dataIvKeyWithVersion.begin(),
                                       dataIvKeyWithVersion.end()),
                               dataIvKeyHmac))
    {
        mState = State::kInternalError;
        return;
    }
    
    mOutStream.write("AES");
    mOutStream.write(uint8_t('\x03'));
    mOutStream.write(uint8_t('\x00'));

    StrRef createdBy("CREATED_BY");
    StrRef creator("kitchenSink");

    mOutStream.write(uint8_t('\x00'));
    mOutStream.write(static_cast<char>(createdBy.length() + creator.length() + 1));
    mOutStream.write(createdBy);
    mOutStream.write(uint8_t('\x00'));
    mOutStream.write(creator);

    mOutStream.write(uint8_t('\x00'));
    mOutStream.write(uint8_t('\x00'));

    mOutStream.write(uint8_t((kdfIterations >> 24) & 0xFF));
    mOutStream.write(uint8_t((kdfIterations >> 16) & 0xFF));
    mOutStream.write(uint8_t((kdfIterations >>  8) & 0xFF));
    mOutStream.write(uint8_t( kdfIterations        & 0xFF));

    mOutStream.write(mIv);
    mOutStream.write(dataIvKeyCrypt);
    mOutStream.write(dataIvKeyHmac);

    if (!mEncryptContext.init(dataKey, dataIv, true) || !mHMAC.init(dataKey))
    {
        mState = State::kInternalError;
    }
}

std::size_t CryptoOutStream::write(const DataRef& data)
{
    if (mState != State::kWriting)
    {
        return 0;
    }

    auto currentData = data;

    while (currentData.size() > 0)
    {
        auto written = mDataOut.write(currentData);
        
        if (mDataOut.remaining() == 0)
        {
            std::array<uint8_t, Crypto::kAesBlockSize> cryptData;
            std::size_t cryptLen = 0;

            if (!mEncryptContext.update(mData.begin(),
                                        Crypto::kAesBlockSize,
                                        cryptData.begin(),
                                        Crypto::kAesBlockSize,
                                        cryptLen))
            {
                mState = State::kInternalError;
                return 0;
            }
            
            auto cryptDataRef(DataRef(cryptData.begin(), cryptData.begin() + cryptLen));
            
            if (!mHMAC.update(cryptDataRef))
            {
                mState = State::kInternalError;
                return 0;
            }
            mOutStream.write(cryptDataRef);

            mDataOut.reset();
        }
        
        currentData = DataRef(currentData.begin() + written, currentData.end());
    }
    
    return data.size();
}

void CryptoOutStream::flush()
{
    if (mState != State::kWriting)
    {
        return;
    }

    // Pass any partial plaintext to PSA so it can be included in the PKCS#7 padding block.
    auto filledBytes(mDataOut.position());

    if (filledBytes > 0)
    {
        std::array<uint8_t, Crypto::kAesBlockSize> unused;
        std::size_t unusedLen = 0;

        if (!mEncryptContext.update(mData.begin(),
                                    filledBytes,
                                    unused.begin(),
                                    unused.size(),
                                    unusedLen))
        {
            mState = State::kInternalError;
            return;
        }
    }

    // PSA adds PKCS#7 padding and outputs the final encrypted block.
    std::array<uint8_t, Crypto::kAesBlockSize * 2> cryptData;
    std::size_t cryptLen = 0;

    if (!mEncryptContext.finish(cryptData.begin(), cryptData.size(), cryptLen))
    {
        mState = State::kInternalError;
        return;
    }

    auto cryptDataRef(DataRef(cryptData.begin(), cryptData.begin() + cryptLen));

    if (!mHMAC.update(cryptDataRef))
    {
        mState = State::kInternalError;
        return;
    }

    mOutStream.write(cryptDataRef);

    Crypto::HMAC hmac;

    if (!mHMAC.finish(hmac))
    {
        mState = State::kInternalError;
        return;
    }

    mOutStream.write(hmac);

    mState = State::kFlushed;
}

