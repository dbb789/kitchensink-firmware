#include "crypto/cryptooutstream.h"

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
    , mDataIv(dataIv)
    , mDataKey(dataKey)
    , mData()
    , mDataOut(mData)
    , mState(State::kWriting)
{
    writeHeader(kdfIterations);

    if (mState == State::kWriting && !mHMAC.init(mDataKey))
    {
        mState = State::kInternalError;
    }
}

CryptoOutStream::~CryptoOutStream()
{
    flush();
}

void CryptoOutStream::writeHeader(uint32_t kdfIterations)
{
    Crypto::Key key;
    
    if (!CryptoUtil::pbkdf2HmacSha512(mPassword, mSuffix, mIv, kdfIterations, key))
    {
        mState = State::kInternalError;
        return;
    }

    std::array<uint8_t, sizeof(mDataIv) + sizeof(mDataKey)> dataIvKey;

    ArrayUtil<decltype(dataIvKey)>::join(mDataIv, mDataKey, dataIvKey);
    
    std::array<uint8_t, sizeof(mDataIv) + sizeof(mDataKey)> dataIvKeyCrypt;

    Crypto::IV nextIv;
    if (!CryptoUtil::encrypt(key,
                             mIv,
                             dataIvKey,
                             dataIvKeyCrypt,
                             nextIv))
    {
        mState = State::kInternalError;
        return;
    }

    // HMAC input is the encrypted IV+Key followed by the version byte 0x03.
    std::array<uint8_t, sizeof(mDataIv) + sizeof(mDataKey) + 1> dataIvKeyWithVersion;
    std::copy(dataIvKeyCrypt.begin(), dataIvKeyCrypt.end(), dataIvKeyWithVersion.begin());
    dataIvKeyWithVersion[sizeof(mDataIv) + sizeof(mDataKey)] = 0x03;

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
            
            if (!CryptoUtil::encrypt(mDataKey,
                                     mDataIv,
                                     Crypto::kAesBlockSize,
                                     mData.begin(),
                                     cryptData.begin(),
                                     mDataIv))
            {
                mState = State::kInternalError;
                return 0;
            }
            
            auto cryptDataRef(DataRef(cryptData.begin(), cryptData.end()));
            
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

    // PKCS#7 padding: always emit a final full block, padded with N bytes of
    // value N, where N is the number of padding bytes (1-16).
    auto filledBytes(mDataOut.position() % Crypto::kAesBlockSize);
    uint8_t padLen = (filledBytes == 0) ? Crypto::kAesBlockSize
                                        : Crypto::kAesBlockSize - filledBytes;

    std::fill(mData.begin() + filledBytes, mData.end(), padLen);

    std::array<uint8_t, Crypto::kAesBlockSize> cryptData;

    if (!CryptoUtil::encrypt(mDataKey,
                             mDataIv,
                             Crypto::kAesBlockSize,
                             mData.begin(),
                             cryptData.begin(),
                             mDataIv))
    {
        mState = State::kInternalError;
        return;
    }

    auto cryptDataRef(DataRef(cryptData.begin(), cryptData.end()));

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

