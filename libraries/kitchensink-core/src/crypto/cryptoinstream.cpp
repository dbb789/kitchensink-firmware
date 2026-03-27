#include "crypto/cryptoinstream.h"

#include "crypto/cryptotypes.h"
#include "crypto/cryptoutil.h"
#include "types/arrayutil.h"
#include "types/arrayoutstream.h"

CryptoInStream::CryptoInStream(InStream&     inStream,
                               const StrRef& password,
                               const StrRef& suffix)
    : mInStream(inStream)
    , mPassword(password)
    , mSuffix(suffix)
    , mState(State::kReading)
{
    readHeader();
}

std::size_t CryptoInStream::read(OutStream& os, std::size_t len)
{
    if (mState == State::kReading)
    {
        do
        {
            mInStream.read(mInBuffer, mInBuffer.remaining());
        }
        while (readBlock() && mState == State::kReading);
    }
    
    return mOutBuffer.read(os, len);
}

void CryptoInStream::readHeader()
{
    std::array<uint8_t, 128> buffer;
    ArrayOutStream out(buffer);

    // Header
    
    if (mInStream.read(out, 3) != 3)
    {
        mState = State::kTruncated;
        return;
    }
    
    if (out.data() != "AES")
    {
        mState = State::kBadHeader;
        return;
    }
    
    // File version
    
    out.reset();
    if (mInStream.read(out, 1) != 1)
    {
        mState = State::kTruncated;
        return;
    }
    
    if (out.data() != uint8_t('\x03'))
    {
        mState = State::kBadVersion;
        return;
    }

    // Reserved
    
    out.reset();
    if (mInStream.read(out, 1) != 1)
    {
        mState = State::kTruncated;
        return;
    }
    
    if (out.data() != uint8_t('\x00'))
    {
        mState = State::kCorrupted;
        return;
    }

    // Skip Extensions

    std::size_t extLen(0);
    
    do
    {
        out.reset();
        if (mInStream.read(out, 2) != 2)
        {
            mState = State::kTruncated;
            return;
        }

        extLen = (std::size_t(out.data()[0]) << 8 |
                  std::size_t(out.data()[1]));

        out.reset();
        if (mInStream.read(out, extLen) != extLen)
        {
            mState = State::kTruncated;
            return;
        }
    } while (extLen != 0);

    // KDF iteration count (v3)

    uint32_t kdfIterations(0);

    {
        out.reset();
        if (mInStream.read(out, 4) != 4)
        {
            mState = State::kTruncated;
            return;
        }

        kdfIterations = (uint32_t(out.data()[0]) << 24 |
                         uint32_t(out.data()[1]) << 16 |
                         uint32_t(out.data()[2]) << 8  |
                         uint32_t(out.data()[3]));
    }

    // IV
    
    Crypto::IV iv;
    
    {
        ArrayOutStream out(iv);
    
        if (mInStream.read(out, iv.size()) != iv.size())
        {
            mState = State::kTruncated;
            return;
        }
    }

    // Encrypted IV + Key
    
    std::array<uint8_t, Crypto::kAesBlockSize + Crypto::kAesKeyLen> dataIvKeyCrypt;

    {
        ArrayOutStream out(dataIvKeyCrypt);
        
        if (mInStream.read(out, dataIvKeyCrypt.size()) != dataIvKeyCrypt.size())
        {
            mState = State::kTruncated;
            return;
        }
    }

    // IV + Key HMAC
    
    Crypto::HMAC dataIvKeyHmac;

    {
        ArrayOutStream out(dataIvKeyHmac);
        
        if (mInStream.read(out, dataIvKeyHmac.size()) != dataIvKeyHmac.size())
        {
            mState = State::kTruncated;
            return;
        }
    }

    // Verify HMAC
    Crypto::Key key;
    
    if (!CryptoUtil::pbkdf2HmacSha512(mPassword, mSuffix, iv, kdfIterations, key))
    {
        mState = State::kInternalError;
        return;
    }

    // HMAC input is the encrypted IV+Key followed by the version byte 0x03.
    std::array<uint8_t, Crypto::kAesBlockSize + Crypto::kAesKeyLen + 1> dataIvKeyWithVersion;
    std::copy(dataIvKeyCrypt.begin(), dataIvKeyCrypt.end(), dataIvKeyWithVersion.begin());
    dataIvKeyWithVersion[Crypto::kAesBlockSize + Crypto::kAesKeyLen] = 0x03;

    Crypto::HMAC expectedDataIvKeyHmac;

    if (!HMACContext::generate(key,
                               DataRef(dataIvKeyWithVersion.begin(),
                                       dataIvKeyWithVersion.end()),
                               expectedDataIvKeyHmac))
    {
        mState = State::kInternalError;
        return;
    }
    
    if (dataIvKeyHmac != expectedDataIvKeyHmac)
    {
        mState = State::kBadHmac;
        return;
    }
    
    std::array<uint8_t, Crypto::kAesBlockSize + Crypto::kAesKeyLen> dataIvKey;

    Crypto::IV nextIv;
    
    if (!CryptoUtil::decrypt(key,
                             iv,
                             dataIvKeyCrypt,
                             dataIvKey,
                             nextIv))
    {
        mState = State::kCorrupted;
        return;
    }
    
    ArrayUtil<decltype(dataIvKey)>::split(dataIvKey, mDataIv, mDataKey);

    if (!mHMAC.init(mDataKey))
    {
        mState = State::kInternalError;
        return;
    }
}

bool CryptoInStream::readBlock()
{
    static constexpr size_t SuffixLen = 32;
    
    // Buffer underflow - this shouldn't happen.
    if (mInBuffer.size() < Crypto::kAesBlockSize)
    {
        mState = State::kTruncated;
        
        return false;
    }

    // Stop reading blocks until output buffer has been consumed.
    if (mOutBuffer.remaining() < Crypto::kAesBlockSize)
    {
        return false;
    }

    // Now decrypt a single block.
    
    std::array<uint8_t, Crypto::kAesBlockSize> inBlock;
    std::array<uint8_t, Crypto::kAesBlockSize> outBlock;
    ArrayOutStream inBlockStream(inBlock);
    
    mInBuffer.read(inBlockStream, Crypto::kAesBlockSize);

    if (!mHMAC.update(DataRef(inBlock.begin(), inBlock.end())))
    {
        mState = State::kInternalError;
        return false;
    }
    
    if (!CryptoUtil::decrypt(mDataKey,
                             mDataIv,
                             Crypto::kAesBlockSize,
                             inBlock.begin(),
                             outBlock.begin(),
                             mDataIv))
    {
        mState = State::kCorrupted;
        return false;
    }
    
    auto blockSize(Crypto::kAesBlockSize);

    bool readMore = true;

    // If the remaining content of the buffer is the exact size of the suffix
    // then we're at the end - consume the suffix.
    if (mInBuffer.size() == SuffixLen)
    {
        std::array<uint8_t, SuffixLen> suffix;
        ArrayOutStream suffixStream(suffix);
        
        mInBuffer.read(suffixStream, suffix.size());

        // PKCS#7 unpadding: the last byte of the decrypted block is the pad length.
        uint8_t padLen = outBlock[Crypto::kAesBlockSize - 1];

        if (padLen == 0 || padLen > Crypto::kAesBlockSize)
        {
            mState = State::kCorrupted;
            return false;
        }

        for (std::size_t i = Crypto::kAesBlockSize - padLen; i < Crypto::kAesBlockSize; ++i)
        {
            if (outBlock[i] != padLen)
            {
                mState = State::kCorrupted;
                return false;
            }
        }

        blockSize = Crypto::kAesBlockSize - padLen;

        Crypto::HMAC dataHmac;
            
        std::copy(suffix.begin(),
                  suffix.end(),
                  dataHmac.begin());

        Crypto::HMAC expectedHmac;
        
        if (!mHMAC.finish(expectedHmac))
        {
            mState = State::kInternalError;
            return false;
        }

        if (dataHmac != expectedHmac)
        {
            mState = State::kBadDataHmac;
        }
        else
        {
            mState = State::kValidated;
        }
        
        readMore = false;
    }

    auto blockData(DataRef(outBlock.begin(), outBlock.begin() + blockSize));
    
    mOutBuffer.write(blockData);

    return readMore;
}
