#include "crypto/cryptooutstream.h"

#include "types/arrayutil.h"
#include "types/stroutstream.h"
#include "config.h"

CryptoOutStream::CryptoOutStream(OutStream&         outStream,
                                 const StrRef&      password,
                                 const Crypto::IV&  iv,
                                 const Crypto::IV&  dataIv,
                                 const Crypto::Key& dataKey)
    : mOutStream(outStream)
    , mPassword(password)
    , mIv(iv)
    , mDataIv(dataIv)
    , mDataKey(dataKey)
    , mData()
    , mDataOut(mData)
    , mState(State::kWriting)
{
    writeHeader();

    if (mState == State::kWriting && !mHMAC.init(mDataKey))
    {
        mState = State::kInternalError;
    }
}

CryptoOutStream::~CryptoOutStream()
{
    flush();
}

void CryptoOutStream::writeHeader()
{
    Crypto::Key key;
    
    if (!CryptoUtil::stretch(mPassword, mIv, key))
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

    Crypto::HMAC dataIvKeyHmac;
    
    if (!HMACContext::generate(key,
                               DataRef(dataIvKeyCrypt.begin(),
                                       dataIvKeyCrypt.end()),
                               dataIvKeyHmac))
    {
        mState = State::kInternalError;
        return;
    }
    
    mOutStream.write("AES");
    mOutStream.write(uint8_t('\x02'));
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

    auto blockOffset(mDataOut.position() % Crypto::kAesBlockSize);

    if (mDataOut.position() > 0)
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
            return;
        }

        auto cryptDataRef(DataRef(cryptData.begin(), cryptData.end()));

        if (!mHMAC.update(cryptDataRef))
        {
            mState = State::kInternalError;
            return;
        }
        mOutStream.write(cryptDataRef);
        
        mDataOut.reset();
    }
    
    mOutStream.write(uint8_t(blockOffset));

    auto hmac(mHMAC.finish());
    
    mOutStream.write(hmac);

    mState = State::kFlushed;
}

