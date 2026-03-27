#include "crypto/aesencryptcontext.h"

#include "crypto/cryptoutil.h"

AESEncryptContext::AESEncryptContext()
    : mState(State::kUninitialized)
    , mOperation(PSA_CIPHER_OPERATION_INIT)
    , mKeyId(MBEDTLS_SVC_KEY_ID_INIT)
{ }

AESEncryptContext::~AESEncryptContext()
{
    if (mState != State::kUninitialized && mState != State::kFinished)
    {
        psa_cipher_abort(&mOperation);
        psa_destroy_key(mKeyId);
    }
}

bool AESEncryptContext::init(const Crypto::Key& key,
                             const Crypto::IV&  iv,
                             bool               pkcs7)
{
    if (mState != State::kUninitialized)
    {
        return false;
    }

    CryptoUtil::initializeLibrary();

    psa_algorithm_t algorithm = pkcs7 ? PSA_ALG_CBC_PKCS7 : PSA_ALG_CBC_NO_PADDING;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, algorithm);
    psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(key.size()));

    if (psa_import_key(&attributes, key.data(), key.size(), &mKeyId) != PSA_SUCCESS)
    {
        return false;
    }

    if (psa_cipher_encrypt_setup(&mOperation, mKeyId, algorithm) != PSA_SUCCESS)
    {
        psa_destroy_key(mKeyId);
        return false;
    }

    if (psa_cipher_set_iv(&mOperation, iv.data(), iv.size()) != PSA_SUCCESS)
    {
        psa_cipher_abort(&mOperation);
        psa_destroy_key(mKeyId);
        return false;
    }

    mState = State::kInitialized;

    return true;
}

bool AESEncryptContext::update(const uint8_t* in,
                               std::size_t    inLen,
                               uint8_t*       out,
                               std::size_t    outBufLen,
                               std::size_t&   outLen)
{
    if (mState != State::kInitialized)
    {
        return false;
    }

    outLen = 0;

    if (psa_cipher_update(&mOperation, in, inLen, out, outBufLen, &outLen) != PSA_SUCCESS)
    {
        mState = State::kInternalError;
        return false;
    }

    return true;
}

bool AESEncryptContext::finish()
{
    uint8_t discard[Crypto::kAesBlockSize];
    std::size_t discardLen = 0;

    return finish(discard, sizeof(discard), discardLen);
}

bool AESEncryptContext::finish(uint8_t*     out,
                               std::size_t  maxOut,
                               std::size_t& outLen)
{
    if (mState != State::kInitialized)
    {
        return false;
    }

    outLen = 0;

    if (psa_cipher_finish(&mOperation, out, maxOut, &outLen) != PSA_SUCCESS)
    {
        mState = State::kInternalError;
        psa_destroy_key(mKeyId);
        return false;
    }

    mState = State::kFinished;

    psa_destroy_key(mKeyId);

    return true;
}
