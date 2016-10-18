#include "org/conscrypt/CryptoUpcalls.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Security::CSecurity;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::IKey;
using Elastos::Security::ISecurity;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::Interfaces::IDSAPrivateKey;
using Elastos::Security::Interfaces::IECPrivateKey;
using Elastos::Security::Interfaces::IRSAPrivateKey;
using Elastos::Utility::Logging::Logger;
using Elastosx::Crypto::CCipherHelper;
using Elastosx::Crypto::ICipher;
using Elastosx::Crypto::ICipherHelper;

namespace Org {
namespace Conscrypt {

const String CryptoUpcalls::RSA_CRYPTO_ALGORITHM("RSA/ECB/PKCS1Padding");

ECode CryptoUpcalls::GetExternalProvider(
    /* [in] */ const String& algorithm,
    /* [out] */ IProvider** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IProvider> selectedProvider;
    AutoPtr<ArrayOf<IProvider*> > providers;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProviders(algorithm, (ArrayOf<IProvider*>**)&providers);

    for (Int32 i = 0; i < providers->GetLength(); ++i) {
        AutoPtr<IProvider> p = (*providers)[i];
// TODO: Need getClass
        // if (!p.getClass().getClassLoader().equals(CryptoUpcalls.class.getClassLoader())) {
        //     selectedProvider = p;
        //     break;
        // }
    }
    if (selectedProvider == NULL) {
        // System.err.println("Could not find external provider for algorithm: " + algorithm);
        Logger::E("CryptoUpcalls", "Could not find external provider for algorithm: %s",
                algorithm.string());
    }
    *result = selectedProvider;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CryptoUpcalls::RawSignDigestWithPrivateKey(
    /* [in] */ IPrivateKey* javaKey,
    /* [in] */ ArrayOf<Byte>* message,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    // Get the raw signature algorithm for this key type.
    String algorithm;
    // Hint: Algorithm names come from:
    // http://docs.oracle.com/javase/6/docs/technotes/guides/security/StandardNames.html
    if (IRSAPrivateKey::Probe(javaKey) != NULL) {
        // IMPORTANT: Due to a platform bug, this will throw
        // NoSuchAlgorithmException
        // on Android 4.0.x and 4.1.x. Fixed in 4.2 and higher.
        // See https://android-review.googlesource.com/#/c/40352/
        algorithm = String("NONEwithRSA");
    }
    else if (IDSAPrivateKey::Probe(javaKey) != NULL) {
        algorithm = String("NONEwithDSA");
    }
    else if (IECPrivateKey::Probe(javaKey) != NULL) {
        algorithm = String("NONEwithECDSA");
    }
    else {
        // throw new RuntimeException("Unexpected key type: " + javaKey.toString());
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IProvider> p;
    GetExternalProvider(String("Signature.") + algorithm, (IProvider**)&p);
    if (p == NULL) {
        return NOERROR;
    }

    // Get the Signature for this key.
    AutoPtr<ISignature> signature;
    // try {
    AutoPtr<ISignatureHelper> sigHelper;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&sigHelper);
    sigHelper->GetInstance(algorithm, p, (ISignature**)&signature);
    // } catch (NoSuchAlgorithmException e) {
    //     ;
    // }

    if (signature == NULL) {
        String str;
        IKey::Probe(javaKey)->GetAlgorithm(&str);
        // System.err.println("Unsupported private key algorithm: " + javaKey.getAlgorithm());
        Logger::E("CryptoUpcalls", "Unsupported private key algorithm: %s",
                str.string());
        *result = NULL;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // Sign the message.
    // try {
    signature->InitSign(javaKey);
    signature->Update(message);
    return signature->Sign(result);
    // } catch (Exception e) {
    //     System.err.println("Exception while signing message with " + javaKey.getAlgorithm()
    //             + " private key:");
    //     e.printStackTrace();
    //     return NULL;
    // }
}

ECode CryptoUpcalls::RawCipherWithPrivateKey(
    /* [in] */ IPrivateKey* javaKey,
    /* [in] */ Boolean encrypt,
    /* [in] */ ArrayOf<Byte>* input,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    if (IRSAPrivateKey::Probe(javaKey) == NULL) {
        // System.err.println("Unexpected key type: " + javaKey.toString());
        String str;
        IObject::Probe(javaKey)->ToString(&str);
        Logger::E("CryptoUpcalls", "Unexpected key type: %s", str.string());
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IProvider> p;
    GetExternalProvider(String("Cipher.") + RSA_CRYPTO_ALGORITHM, (IProvider**)&p);
    if (p == NULL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ICipher> c;
    // try {
    AutoPtr<ICipherHelper> helper;
    CCipherHelper::AcquireSingleton((ICipherHelper**)&helper);
    helper->GetInstance(RSA_CRYPTO_ALGORITHM, p, (ICipher**)&c);
    // } catch (NoSuchAlgorithmException e) {
    //     ;
    // } catch (NoSuchPaddingException e) {
    //     ;
    // }

    if (c == NULL) {
        // System.err.println("Unsupported private key algorithm: " + javaKey.getAlgorithm());
        String str;
        IKey::Probe(javaKey)->GetAlgorithm(&str);
        Logger::E("CryptoUpcalls", "Unsupported private key algorithm: %s", str.string());
        return NOERROR;
    }

    // try {
    c->Init(encrypt ? ICipher::ENCRYPT_MODE : ICipher::DECRYPT_MODE, IKey::Probe(javaKey));
    return c->DoFinal(input, result);
    // } catch (Exception e) {
    //     System.err.println("Exception while ciphering message with " + javaKey.getAlgorithm()
    //             + " private key:");
    //     e.printStackTrace();
    //     return NULL;
    // }
}

} // namespace Conscrypt
} // namespace Org
