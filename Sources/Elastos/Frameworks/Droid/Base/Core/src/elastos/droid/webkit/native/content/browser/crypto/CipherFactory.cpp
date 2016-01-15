
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include "elastos/droid/webkit/native/content/browser/crypto/CipherFactory.h"
#include "elastos/droid/webkit/native/content/browser/crypto/ByteArrayGenerator.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::AutoLock;
using Elastosx::Crypto::ICipherHelper;
// TODO using Elastosx::Crypto::CCipherHelper;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::IKeyGenerator;
using Elastosx::Crypto::IKeyGeneratorHelper;
using Elastosx::Crypto::CKeyGeneratorHelper;
using Elastosx::Crypto::Spec::IIvParameterSpec;
// TODO using Elastosx::Crypto::Spec::CIvParameterSpec;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Security::EIID_IKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::ISecureRandomHelper;
// TODO using Elastos::Security::CSecureRandomHelper;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::EIID_ICallable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Crypto {

//==================================================================
//                  CipherFactory::CipherData
//==================================================================

CipherFactory::CipherData::CipherData(
    /* [in] */ IKey* _key,
    /* [in] */ ArrayOf<Byte>* _iv)
    : key(_key)
    , iv(_iv)
{
}

//==================================================================
//                  CipherFactory::LazyHolder
//==================================================================

AutoPtr<CipherFactory> CipherFactory::LazyHolder::sInstance = new CipherFactory();

//==================================================================
//                  CipherFactory::InnerCallable
//==================================================================

CAR_INTERFACE_IMPL(CipherFactory::InnerCallable, Object, ICallable);

CipherFactory::InnerCallable::InnerCallable(
    /* [in] */ CipherFactory* owner)
    : mOwner(owner)
{
}

ECode CipherFactory::InnerCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    // Poll random data to generate initialization parameters for the Cipher.
    AutoPtr< ArrayOf<Byte> > seed, iv;
    // try {
        seed = mOwner->mRandomNumberProvider->GetBytes(NUM_BYTES);
        iv = mOwner->mRandomNumberProvider->GetBytes(NUM_BYTES);
    // } catch (Exception e) {
    //     Log.e(TAG, "Couldn't get generator data.");
    //     return null;
    // }

    // try {
        // Old versions of SecureRandom do not seed themselves as securely as possible.
        // This workaround should suffice until the fixed version is deployed to all
        // users. The seed comes from RandomNumberProvider.getBytes(), which reads
        // from /dev/urandom, which is as good as the platform can get.
        //
        // TODO(palmer): Consider getting rid of this once the updated platform has
        // shipped to everyone. Alternately, leave this in as a defense against other
        // bugs in SecureRandom.
        AutoPtr<ISecureRandomHelper> secureRandomHelper;
        assert(0);
        // TODO
        // CSecureRandomHelper::AcquireSingleton((ISecureRandomHelper**)&secureRandomHelper);
        AutoPtr<ISecureRandom> random;
        secureRandomHelper->GetInstance(String("SHA1PRNG"), (ISecureRandom**)&random);
        random->SetSeed(seed);

        AutoPtr<IKeyGeneratorHelper> keyGeneratorHelper;
        assert(0);
        // TODO
        // CKeyGeneratorHelper::AcquireSingleton((IKeyGeneratorHelper**)&keyGeneratorHelper);
        AutoPtr<IKeyGenerator> generator;
        keyGeneratorHelper->GetInstance(String("AES"), (IKeyGenerator**)&generator);
        generator->Init(128, random);
        AutoPtr<ISecretKey> secretKey;
        generator->GenerateKey((ISecretKey**)&secretKey);
        AutoPtr<IKey> key = IKey::Probe(secretKey);
        AutoPtr<CipherData> cipherData = new CipherData(key, iv);
        *result = (IInterface*)(IObject*)cipherData;
        REFCOUNT_ADD(*result);
    // } catch (GeneralSecurityException e) {
    //     Log.e(TAG, "Couldn't get generator instances.");
    //     return null;
    // }

    return NOERROR;
}

//==================================================================
//                        CipherFactory
//==================================================================

const String CipherFactory::TAG("CipherFactory");
const Int32 CipherFactory::NUM_BYTES;

const String CipherFactory::BUNDLE_IV("org.chromium.content.browser.crypto.CipherFactory.IV");
const String CipherFactory::BUNDLE_KEY("org.chromium.content.browser.crypto.CipherFactory.KEY");

CipherFactory::CipherFactory()
{
    mRandomNumberProvider = new ByteArrayGenerator();
}

/** @return The Singleton instance. Creates it if it doesn't exist. */
AutoPtr<CipherFactory> CipherFactory::GetInstance()
{
    return LazyHolder::sInstance;
}

/**
 * Creates a secure Cipher for encrypting data.
 * This function blocks until data needed to generate a Cipher has been created by the
 * background thread.
 * @param opmode One of Cipher.{ENCRYPT,DECRYPT}_MODE.
 * @return A Cipher, or null if it is not possible to instantiate one.
 */
AutoPtr<ICipher> CipherFactory::GetCipher(
    /* [in] */ Int32 opmode)
{
    AutoPtr<CipherData> data = GetCipherData(TRUE);

    if (data != NULL) {
        // try {
            AutoPtr<ICipher> cipher;
            AutoPtr<ICipherHelper> helper;
            assert(0);
            // TODO
            // CCipherHelper::AcquireSingleton((ICipherHelper**)&helper);
            helper->GetInstance(String("AES/CBC/PKCS5Padding"), (ICipher**)&cipher);
            AutoPtr<IIvParameterSpec> ivParameterSpec;
            assert(0);
            // TODO
            // CIvParameterSpec::New((IIvParameterSpec**)&ivParameterSpec);
            AutoPtr<IAlgorithmParameterSpec> algorithmParameterSpec =
                   IAlgorithmParameterSpec::Probe(ivParameterSpec);
            cipher->Init(opmode, data->key, algorithmParameterSpec);
            return cipher;
        // } catch (GeneralSecurityException e) {
        //     // Can't do anything here.
        // }
    }

    Slogger::E(TAG, "Error in creating cipher instance.");

    return NULL;
}

/**
 * Returns data required for generating the Cipher.
 * @param generateIfNeeded Generates data on the background thread, blocking until it is done.
 * @return Data to use for the Cipher, null if it couldn't be generated.
 */
AutoPtr<CipherFactory::CipherData> CipherFactory::GetCipherData(
    /* [in] */ Boolean generateIfNeeded)
{
    if (mData == NULL && generateIfNeeded) {
        // Ideally, this task should have been started way before this.
        TriggerKeyGeneration();

        // Grab the data from the task.
        AutoPtr<CipherData> data;
        // try {
            mDataGenerator->Get((IInterface**)&data);
        // } catch (InterruptedException e) {
        //     throw new RuntimeException(e);
        // } catch (ExecutionException e) {
        //     throw new RuntimeException(e);
        // }

        // Only the first thread is allowed to save the data.
        //synchronized(mDataLock)
        {
            AutoLock lock(mDataLock);
            if (mData == NULL) {
                mData = data;
            }
        }
    }
    return mData;
}

/**
 * Creates a Callable that generates the data required to create a Cipher. This is done on a
 * background thread to prevent blocking on the I/O required for
 * {@link ByteArrayGenerator#getBytes(int)}.
 * @return Callable that generates the Cipher data.
 */
AutoPtr<ICallable> CipherFactory::CreateGeneratorCallable()
{
    AutoPtr<ICallable> callable = new InnerCallable(this);
    return callable;
}

/**
 * Generates the encryption key and IV on a background thread (if necessary).
 * Should be explicitly called when the Activity determines that it will need a Cipher rather
 * than immediately calling {@link CipherFactory#getCipher(int)}.
 */
void CipherFactory::TriggerKeyGeneration()
{
    if (mData != NULL) return;

    //synchronized(mDataLock)
    {
        AutoLock lock(mDataLock);
        if (mDataGenerator == NULL) {
            mDataGenerator = new FutureTask(CreateGeneratorCallable());
            AsyncTask::THREAD_POOL_EXECUTOR->Execute(mDataGenerator);
        }
    }
}

/**
 * Saves the encryption data in a bundle. Expected to be called when an Activity saves its state
 * before being sent to the background.
 *
 * The IV *could* go into the first block of the payload. However, since the staleness of the
 * data is determined by whether or not it's able to be decrypted, the IV should not be read
 * from it.
 *
 * @param outState The data bundle to store data into.
 */
void CipherFactory::SaveToBundle(
    /* [in] */ IBundle* outState)
{
    AutoPtr<CipherData> data = GetCipherData(false);
    if (data == NULL) return;

    AutoPtr< ArrayOf<Byte> > wrappedKey;
    data->key->GetEncoded((ArrayOf<Byte>**)&wrappedKey);
    if (wrappedKey != NULL && data->iv != NULL) {
        outState->PutByteArray(BUNDLE_KEY, wrappedKey);
        outState->PutByteArray(BUNDLE_IV, data->iv);
    }
}

/**
 * Restores the encryption key from the given Bundle. Expected to be called when an Activity is
 * being restored after being killed in the background. If the Activity was explicitly killed by
 * the user, Android gives no Bundle (and therefore no key).
 *
 * @param savedInstanceState Bundle containing the Activity's previous state. Null if the user
 *                           explicitly killed the Activity.
 * @return                   True if the data was restored successfully from the Bundle, or if
 *                           the CipherData in use matches the Bundle contents.
 *
 */
Boolean CipherFactory::RestoreFromBundle(
    /* [in] */ IBundle* savedInstanceState)
{
    if (savedInstanceState == NULL) return FALSE;

    AutoPtr< ArrayOf<Byte> > wrappedKey;
    savedInstanceState->GetByteArray(BUNDLE_KEY, (ArrayOf<Byte>**)&wrappedKey);
    AutoPtr< ArrayOf<Byte> > iv;
    savedInstanceState->GetByteArray(BUNDLE_IV, (ArrayOf<Byte>**)&iv);
    if (wrappedKey == NULL || iv == NULL) return FALSE;

    // try {
        AutoPtr<IKey> bundledKey;
        assert(0);
        // TODO
        // CSecretKeySpec(wrappedKey, String("AES"), (IKey**)&bundledKey);

        {
            AutoLock lock(mDataLock);
            if (mData == NULL) {
                mData = new CipherData(bundledKey, iv);
                return TRUE;
            }
            else if (/* TODO mData->key->Equals(bundledKey) && Arrays.equals(mData.iv, iv)*/FALSE) {
                return TRUE;
            }
            else {
                Slogger::E(TAG, "Attempted to restore different cipher data.");
            }
        }
    // } catch (IllegalArgumentException e) {
    //     Log.e(TAG, "Error in restoring the key from the bundle.");
    // }

    return FALSE;
}

/**
 * Overrides the random number generated that is normally used by the class.
 * @param mockProvider Should be used to provide non-random data.
 */
void CipherFactory::SetRandomNumberProviderForTests(
    /* [in] */ ByteArrayGenerator* mockProvider)
{
    mRandomNumberProvider = mockProvider;
}

} // namespace Crypto
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
