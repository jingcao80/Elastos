
#include "Elastos.CoreLibrary.Security.h"
#include "CMac.h"
#include "AutoLock.h"

using Elastos::Security::ISecurity;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CMac)
CAR_INTERFACE_IMPL(CMac, Object, IMac)

String CMac::mSERVICE = String("Mac");
//TODO: Need IEngine
//AutoPtr<IEngine> CMac::mENGINE;

CMac::CMac()
    : mAlgorithm(String(NULL))
    , mIsInitMac(FALSE)
{
//TODO: Need IEngine
    // CEngine::New(mSERVICE, (IEngine**)&mEngine);
}

ECode CMac::constructor(
    /* [in] */ IMacSpi* macSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mSpecifiedProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = macSpi;
    mIsInitMac = FALSE;
    return NOERROR;
}

ECode CMac::GetAlgorithm(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAlgorithm;
    return NOERROR;
}

ECode CMac::GetProvider(
    /* [out] */ IProvider ** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CMac::GetMacLength(
    /* [out] */ Int32 * len)
{
    VALIDATE_NOT_NULL(len)
    *len = 0;
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    return spi->EngineGetMacLength(len);
}

ECode CMac::Init(
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameterSpec* spec)
{
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    AutoPtr<IMacSpi> spi;
    GetSpi(key, (IMacSpi**)&spi);
    spi->EngineInit(key, spec);
    mIsInitMac = TRUE;
    return NOERROR;
}

ECode CMac::Init(
    /* [in] */ IKey * key)
{
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    // try {
        AutoPtr<IMacSpi> spi;
        GetSpi((IMacSpi**)&spi);
        spi->EngineInit(key, NULL);
        mIsInitMac = TRUE;
    // } catch (InvalidAlgorithmParameterException e) {
    //     throw new RuntimeException(e);
    // }
    return NOERROR;
}

ECode CMac::Update(
    /* [in] */ Byte input)
{
    if (!mIsInitMac) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    return spi->EngineUpdate(input);
}

ECode CMac::Update(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    if (!mIsInitMac) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == NULL) {
        return NOERROR;
    }
    if ((offset < 0) || (len < 0) || ((offset + len) > input->GetLength())) {
        // throw new IllegalArgumentException("Incorrect arguments."
        //                                    + " input.length=" + input.length
        //                                    + " offset=" + offset + ", len=" + len);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    return spi->EngineUpdate(input, offset, len);
}

ECode CMac::Update(
    /* [in] */ ArrayOf<Byte> * input)
{
    if (!mIsInitMac) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input != NULL) {
        AutoPtr<IMacSpi> spi;
        GetSpi((IMacSpi**)&spi);
        spi->EngineUpdate(input, 0, input->GetLength());
    }
    return NOERROR;
}

ECode CMac::Update(
    /* [in] */ IByteBuffer * input)
{
    if (!mIsInitMac) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input != NULL) {
        AutoPtr<IMacSpi> spi;
        GetSpi((IMacSpi**)&spi);
        // TODO:
        // spi->EngineUpdate(input);
    } else {
        // throw new IllegalArgumentException("input == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CMac::DoFinal(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (!mIsInitMac) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    return spi->EngineDoFinal(result);
}

ECode CMac::DoFinal(
    /* [in] */ ArrayOf<Byte> * output,
    /* [in] */ Int32 outOffset)
{
    if (!mIsInitMac) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (output == NULL) {
        // throw new ShortBufferException("output == NULL");
        return E_SHORT_BUFFER_EXCEPTION;
    }
    if ((outOffset < 0) || (outOffset >= output->GetLength())) {
        // throw new ShortBufferException("Incorrect outOffset: " + outOffset);
        return E_SHORT_BUFFER_EXCEPTION;
    }
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    Int32 t;
    spi->EngineGetMacLength(&t);
    if (t > (output->GetLength() - outOffset)) {
        // throw new ShortBufferException("Output buffer is short. Needed " + t + " bytes.");
        return E_SHORT_BUFFER_EXCEPTION;
    }
    AutoPtr<ArrayOf<Byte> > result;
    spi->EngineDoFinal((ArrayOf<Byte>**)&result);
    // System.arraycopy(result, 0, output, outOffset, result->GetLength());
    output->Copy(outOffset, result, 0, result->GetLength());
    return NOERROR;
}

ECode CMac::DoFinal(
    /* [in] */ ArrayOf<Byte> * input,
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (!mIsInitMac) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    if (input != NULL) {
        spi->EngineUpdate(input, 0, input->GetLength());
    }
    return spi->EngineDoFinal(result);
}

ECode CMac::Reset()
{
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    return spi->EngineReset();
}

ECode CMac::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IMac ** mac)
{
    VALIDATE_NOT_NULL(mac)
    *mac = NULL;
    return GetMac(algorithm, NULL, mac);
}

ECode CMac::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IMac ** mac)
{
    VALIDATE_NOT_NULL(mac)
    *mac = NULL;
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException("Provider is NULL or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
//TODO: Need CSecurity
    // CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);

    if (impProvider == NULL) {
        // throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetMac(algorithm, impProvider, mac);
}

ECode CMac::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IMac ** mac)
{
    VALIDATE_NOT_NULL(mac)
    *mac = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return GetMac(algorithm, provider, mac);
}

ECode CMac::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IMacSpi> newSpiImpl;
    AutoPtr<IMacSpi> spi;
    GetSpi((IMacSpi**)&spi);
    if (spi != NULL) {
        ICloneable::Probe(spi)->Clone((IInterface**)&newSpiImpl);
    }
    AutoPtr<CMac> mac;
    CMac::NewByFriend(newSpiImpl, mProvider, mAlgorithm, (CMac**)&mac);
    mac->mIsInitMac = mIsInitMac;
    *result = (IInterface*)(IMac*)mac;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMac::GetMac(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IMac ** mac)
{
    VALIDATE_NOT_NULL(mac)
    *mac = NULL;
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }

//TODO: Need IEngine
    // if (TryAlgorithm(NULL, provider, algorithm) == NULL) {
    //     if (provider == NULL) {
    //         // throw new NoSuchAlgorithmException("No provider found for " + algorithm);
    //         return E_NO_SUCH_ALGORITHM_EXCEPTION;
    //     } else {
    //         // throw new NoSuchAlgorithmException("Provider " + provider.getName()
    //         //         + " does not provide " + algorithm);
    //         return E_NO_SUCH_ALGORITHM_EXCEPTION;
    //     }
    // }

    return CMac::New(NULL, provider, algorithm, mac);
}

//TODO: Need IEngine
// CARAPI(Autoptr<IEngine.SpiAndProvider>) TryAlgorithm(
//     /* [in] */ IKey * key,
//     /* [in] */ IProvider * provider,
//     /* [in] */ const String& algorithm)
// {
//         if (provider != null) {
//             Provider.Service service = provider.getService(SERVICE, algorithm);
//             if (service == null) {
//                 return null;
//             }
//             return tryAlgorithmWithProvider(key, service);
//         }
//         ArrayList<Provider.Service> services = ENGINE.getServices(algorithm);
//         if (services == null) {
//             return null;
//         }
//         for (Provider.Service service : services) {
//             Engine.SpiAndProvider sap = tryAlgorithmWithProvider(key, service);
//             if (sap != null) {
//                 return sap;
//             }
//         }
//         return null;
// }

// CARAPI(Autoptr<IEngine.SpiAndProvider>) TryAlgorithmWithProvider(
//     /* [in] */ IKey * key,
//     /* [in] */ IProvider.Service service)
// {
//     try {
//         if (key != null && !service.supportsParameter(key)) {
//             return null;
//         }

//         Engine.SpiAndProvider sap = ENGINE.getInstance(service, null);
//         if (sap.spi == null || sap.provider == null) {
//             return null;
//         }
//         if (!(sap.spi instanceof MacSpi)) {
//             return null;
//         }
//         return sap;
//     } catch (NoSuchAlgorithmException ignored) {
//     }
//     return null;
// }

/**
 * Makes sure a MacSpi that matches this type is selected.
 */
ECode CMac::GetSpi(
    /* [in] */ IKey* key,
    /* [out] */ IMacSpi** spi)
{
    VALIDATE_NOT_NULL(spi)
    *spi = NULL;
    synchronized (mInitLock) {
        if (mSpiImpl != NULL && mProvider != NULL && key == NULL) {
            *spi = mSpiImpl;
            REFCOUNT_ADD(*spi)
            return NOERROR;
        }

        if (mAlgorithm == NULL) {
            *spi = NULL;
            return NOERROR;
        }

//TODO: Need IEngine
        // AutoPtr<IEngine.SpiAndProvider> sap = TryAlgorithm(key, specifiedProvider, algorithm);
        // if (sap == NULL) {
        //     // throw new ProviderException("No provider for " + getAlgorithm());
        //     return E_PROVIDER_EXCEPTION;
        // }

        // /*
        //  * Set our Spi if we've never been initialized or if we have the Spi
        //  * specified and have a NULL provider.
        //  */
        // if (mSpiImpl == NULL || provider != NULL) {
        //     mSpiImpl = (MacSpi) sap.spi;
        // }
        // mProvider = sap.provider;

        *spi = mSpiImpl;
        REFCOUNT_ADD(*spi)
        return NOERROR;
    }
    return NOERROR;
}

/**
 * Convenience call when the Key is not available.
 */
ECode CMac::GetSpi(
    /* [out] */ IMacSpi** spi)
{
    return GetSpi(NULL, spi);
}

} // Crypto
} // Elastosx