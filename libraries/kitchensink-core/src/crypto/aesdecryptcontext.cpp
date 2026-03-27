#include "crypto/aesdecryptcontext.h"

#include "crypto/cryptoutil.h"

AESDecryptContext::AESDecryptContext()
    : mState(State::kUninitialized)
    , mOperation(PSA_CIPHER_OPERATION_INIT)
    , mKeyId(MBEDTLS_SVC_KEY_ID_INIT)
{ }

AESDecryptContext::~AESDecryptContext()
{
    if (mState != State::kUninitialized && mState != State::kFinished)
    {
        psa_cipher_abort(&mOperation);
        psa_destroy_key(mKeyId);
    }
}

bool AESDecryptContext::init(const Crypto::Key& key, const Crypto::IV& iv)
{
    if (mState != State::kUninitialized)
    {
        return false;
    }

    CryptoUtil::initializeLibrary();

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(key.size()));

    if (psa_import_key(&attributes, key.data(), key.size(), &mKeyId) != PSA_SUCCESS)
    {
        return false;
    }

    if (psa_cipher_decrypt_setup(&mOperation, mKeyId, PSA_ALG_CBC_NO_PADDING) != PSA_SUCCESS)
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

bool AESDecryptContext::update(const uint8_t* in, uint8_t* out, std::size_t size)
{
    if (mState != State::kInitialized)
    {
        return false;
    }

    size_t outputLen = 0;

    if (psa_cipher_update(&mOperation, in, size, out, size, &outputLen) != PSA_SUCCESS)
    {
        mState = State::kInternalError;
        return false;
    }

    return true;
}

bool AESDecryptContext::finish()
{
    if (mState != State::kInitialized)
    {
        return false;
    }

    uint8_t overflow[Crypto::kAesBlockSize];
    size_t finishLen = 0;

    if (psa_cipher_finish(&mOperation, overflow, sizeof(overflow), &finishLen) != PSA_SUCCESS)
    {
        mState = State::kInternalError;
        psa_destroy_key(mKeyId);
        return false;
    }

    mState = State::kFinished;

    psa_destroy_key(mKeyId);

    return true;
}
