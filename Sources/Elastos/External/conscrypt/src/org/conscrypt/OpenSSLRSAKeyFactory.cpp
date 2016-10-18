
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLRSAKeyFactory.h"
#include "NativeCrypto.h"
// #include "OpenSSLRSAPrivateCrtKey.h"
#include "COpenSSLRSAPublicKey.h"
#include "COpenSSLRSAPrivateCrtKey.h"
#include "COpenSSLRSAPrivateKey.h"

using Elastos::Security::EIID_IKeyFactorySpi;
using Elastos::Security::Interfaces::IRSAPrivateKey;
using Elastos::Security::Interfaces::IRSAPublicKey;
using Elastos::Security::Interfaces::IRSAPrivateCrtKey;
using Elastos::Security::Interfaces::IRSAKey;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::EIID_IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CRSAPrivateCrtKeySpec;
using Elastos::Security::Spec::IRSAPrivateCrtKeySpec;
using Elastos::Security::Spec::EIID_IRSAPrivateCrtKeySpec;
using Elastos::Security::Spec::CRSAPrivateKeySpec;
using Elastos::Security::Spec::IRSAPrivateKeySpec;
using Elastos::Security::Spec::EIID_IRSAPrivateKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::EIID_IX509EncodedKeySpec;
using Elastos::Security::Spec::CRSAPublicKeySpec;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Security::Spec::EIID_IRSAPublicKeySpec;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLRSAKeyFactory::
//=========================================
CAR_INTERFACE_IMPL_2(OpenSSLRSAKeyFactory, Object, IKeyFactorySpi, IOpenSSLRSAKeyFactory)

ECode OpenSSLRSAKeyFactory::EngineGeneratePublic(
    /* [in] */ IKeySpec* keySpec,
    /* [out] */ IPublicKey** pubKey)
{
    VALIDATE_NOT_NULL(pubKey)
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == null");
        return NOERROR;
    }

    if (IRSAPublicKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<IOpenSSLRSAPublicKey> result;
        assert(0 && "TODO");
        COpenSSLRSAPublicKey::New(IRSAPublicKeySpec::Probe(keySpec), (IOpenSSLRSAPublicKey**)&result);
        *pubKey = IPublicKey::Probe(result);
        REFCOUNT_ADD(*pubKey)
        return NOERROR;
    }
    else if (IX509EncodedKeySpec::Probe(keySpec) != NULL) {
        assert(0 && "TODO");
        // *pubKey = OpenSSLKey::GetPublicKey(IX509EncodedKeySpec::Probe(keySpec), INativeCrypto::EVP_PKEY_RSA);
        // REFCOUNT_ADD(*pubKey)
        return NOERROR;
    }
    // throw new InvalidKeySpecException("Must use RSAPublicKeySpec or X509EncodedKeySpec; was "
    //         + keySpec.getClass().getName());
    return NOERROR;
}

ECode OpenSSLRSAKeyFactory::EngineGeneratePrivate(
    /* [in] */ IKeySpec* keySpec,
    /* [out] */ IPrivateKey** priKey)
{
    VALIDATE_NOT_NULL(priKey)
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        return NOERROR;
    }

    if (IRSAPrivateCrtKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<IOpenSSLRSAPrivateCrtKey> result;
        COpenSSLRSAPrivateCrtKey::New(IRSAPrivateCrtKeySpec::Probe(keySpec), (IOpenSSLRSAPrivateCrtKey**)&result);
        *priKey = IPrivateKey::Probe(result);
        REFCOUNT_ADD(*priKey)
        return NOERROR;
    }
    else if (IRSAPrivateKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<IOpenSSLRSAPrivateKey> result;
        COpenSSLRSAPrivateKey::New(IRSAPrivateKeySpec::Probe(keySpec), (IOpenSSLRSAPrivateKey**)&result);
        *priKey = IPrivateKey::Probe(result);
        REFCOUNT_ADD(*priKey)
        return NOERROR;
    }
    else if (IPKCS8EncodedKeySpec::Probe(keySpec) != NULL) {
        assert(0 && "TODO");
        // *priKey = OpenSSLKey::GetPrivateKey(IPKCS8EncodedKeySpec::Probe(keySpec),
        //         INativeCrypto::EVP_PKEY_RSA);
        // REFCOUNT_ADD(*priKey)
        return NOERROR;
    }
    // throw new InvalidKeySpecException("Must use RSAPublicKeySpec or PKCS8EncodedKeySpec; was "
    //         + keySpec.getClass().getName());
    return NOERROR;
}

ECode OpenSSLRSAKeyFactory::EngineGetKeySpec(
    /* [in] */ IKey* key,
    /* [in] */ const ClassID& keySpec,
    /* [out] */ IKeySpec** retkeySpec)
{
    VALIDATE_NOT_NULL(retkeySpec)

    if (key == NULL) {
        // throw new InvalidKeySpecException("key == null");
        return NOERROR;
    }

    // if (keySpec == 0) {
    //     // throw new InvalidKeySpecException("keySpec == null");
    //     return NOERROR;
    // }

    String algorithm;
    key->GetAlgorithm(&algorithm);
    if (!algorithm.Equals("RSA")) {
        // throw new InvalidKeySpecException("Key must be a RSA key");
        return NOERROR;
    }

    if ((IRSAPublicKey::Probe(key) != NULL) && (keySpec == EIID_IRSAPublicKeySpec)) {
        AutoPtr<IRSAPublicKey> rsaKey = IRSAPublicKey::Probe(key);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> publicExponent;
        rsaKey->GetPublicExponent((IBigInteger**)&publicExponent);
        AutoPtr<IRSAPublicKeySpec> p;
        CRSAPublicKeySpec::New(modulus, publicExponent, (IRSAPublicKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else if ((IPublicKey::Probe(key) != NULL) && (keySpec == EIID_IRSAPublicKeySpec)) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String strFm;
        key->GetFormat(&strFm);
        if (!strFm.Equals("X.509") || encoded == NULL) {
            // throw new InvalidKeySpecException("Not a valid X.509 encoding");
            return NOERROR;
        }
        AutoPtr<IX509EncodedKeySpec> pKeySpec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&pKeySpec);
        AutoPtr<IPublicKey> pk;
        EngineGeneratePublic(IKeySpec::Probe(pKeySpec), (IPublicKey**)&pk);
        AutoPtr<IRSAPublicKey> rsaKey = IRSAPublicKey::Probe(pk);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> publicExponent;
        rsaKey->GetPublicExponent((IBigInteger**)&publicExponent);
        AutoPtr<IRSAPublicKeySpec> p;
        CRSAPublicKeySpec::New(modulus, publicExponent, (IRSAPublicKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else if ((IRSAPrivateCrtKey::Probe(key) != NULL)
            && (keySpec == EIID_IRSAPrivateCrtKeySpec)) {
        AutoPtr<IRSAPrivateCrtKey> rsaKey = IRSAPrivateCrtKey::Probe(key);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> publicExponent;
        rsaKey->GetPublicExponent((IBigInteger**)&publicExponent);
        AutoPtr<IBigInteger> privateExponent;
        IRSAPrivateKey::Probe(rsaKey)->GetPrivateExponent((IBigInteger**)&privateExponent);
        AutoPtr<IBigInteger> primeP;
        rsaKey->GetPrimeP((IBigInteger**)&primeP);
        AutoPtr<IBigInteger> primeQ;
        rsaKey->GetPrimeQ((IBigInteger**)&primeQ);
        AutoPtr<IBigInteger> primeExponentP;
        rsaKey->GetPrimeExponentP((IBigInteger**)&primeExponentP);
        AutoPtr<IBigInteger> primeExponentQ;
        rsaKey->GetPrimeExponentQ((IBigInteger**)&primeExponentQ);
        AutoPtr<IBigInteger> crtCoefficient;
        rsaKey->GetCrtCoefficient((IBigInteger**)&crtCoefficient);
        AutoPtr<IRSAPrivateCrtKeySpec> p;
        CRSAPrivateCrtKeySpec::New(modulus, publicExponent,
                privateExponent, primeP, primeQ,
                primeExponentP, primeExponentQ,
                crtCoefficient, (IRSAPrivateCrtKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else if (IRSAPrivateCrtKey::Probe(key) != NULL
            && keySpec == EIID_IRSAPrivateKeySpec) {
        AutoPtr<IRSAPrivateCrtKey> rsaKey = IRSAPrivateCrtKey::Probe(key);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> privateExponent;
        IRSAPrivateKey::Probe(rsaKey)->GetPrivateExponent((IBigInteger**)&privateExponent);
        AutoPtr<IRSAPrivateKeySpec> p;
        CRSAPrivateKeySpec::New(modulus, privateExponent, (IRSAPrivateKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else if (IRSAPrivateKey::Probe(key) != NULL
            && keySpec == EIID_IRSAPrivateKeySpec) {
        AutoPtr<IRSAPrivateKey> rsaKey = IRSAPrivateKey::Probe(key);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> privateExponent;
        IRSAPrivateKey::Probe(rsaKey)->GetPrivateExponent((IBigInteger**)&privateExponent);
        AutoPtr<IRSAPrivateKeySpec> p;
        CRSAPrivateKeySpec::New(modulus, privateExponent, (IRSAPrivateKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else if (IPrivateKey::Probe(key) != NULL
            && keySpec == EIID_IRSAPrivateCrtKeySpec) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String fm;
        key->GetFormat(&fm);
        if (!fm.Equals("PKCS#8") || encoded == NULL) {
            // throw new InvalidKeySpecException("Not a valid PKCS#8 encoding");
            return NOERROR;
        }
        AutoPtr<IPKCS8EncodedKeySpec> pKeySpec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&pKeySpec);
        AutoPtr<IPrivateKey> pPriKey;
        EngineGeneratePrivate(IKeySpec::Probe(pKeySpec), (IPrivateKey**)&pPriKey);
        AutoPtr<IRSAPrivateKey> privKey = IRSAPrivateKey::Probe(pPriKey);
        if (IRSAPrivateCrtKey::Probe(privKey) != NULL) {
            AutoPtr<IRSAPrivateCrtKey> rsaKey = IRSAPrivateCrtKey::Probe(privKey);
            AutoPtr<IBigInteger> modulus;
            IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
            AutoPtr<IBigInteger> publicExponent;
            rsaKey->GetPublicExponent((IBigInteger**)&publicExponent);
            AutoPtr<IBigInteger> privateExponent;
            IRSAPrivateKey::Probe(rsaKey)->GetPrivateExponent((IBigInteger**)&privateExponent);
            AutoPtr<IBigInteger> primeP;
            rsaKey->GetPrimeP((IBigInteger**)&primeP);
            AutoPtr<IBigInteger> primeQ;
            rsaKey->GetPrimeQ((IBigInteger**)&primeQ);
            AutoPtr<IBigInteger> primeExponentP;
            rsaKey->GetPrimeExponentP((IBigInteger**)&primeExponentP);
            AutoPtr<IBigInteger> primeExponentQ;
            rsaKey->GetPrimeExponentQ((IBigInteger**)&primeExponentQ);
            AutoPtr<IBigInteger> crtCoefficient;
            rsaKey->GetCrtCoefficient((IBigInteger**)&crtCoefficient);
            AutoPtr<IRSAPrivateCrtKeySpec> p;
            CRSAPrivateCrtKeySpec::New(modulus,
                    publicExponent, privateExponent,
                    primeP, primeQ, primeExponentP,
                    primeExponentQ, crtCoefficient,
                    (IRSAPrivateCrtKeySpec**)&p);
            *retkeySpec = IKeySpec::Probe(p);
            REFCOUNT_ADD(*retkeySpec)
            return NOERROR;
        }
        else {
            // throw new InvalidKeySpecException("Encoded key is not an RSAPrivateCrtKey");
            return NOERROR;
        }
    }
    else if (IPrivateKey::Probe(key) != NULL && keySpec == EIID_IRSAPrivateKeySpec) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String fm;
        key->GetFormat(&fm);
        if (!fm.Equals("PKCS#8") || encoded == NULL) {
            // throw new InvalidKeySpecException("Not a valid PKCS#8 encoding");
            return NOERROR;
        }
        AutoPtr<IPKCS8EncodedKeySpec> pKeySpec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&pKeySpec);
        AutoPtr<IPrivateKey> pPriKey;
        EngineGeneratePrivate(IKeySpec::Probe(pKeySpec), (IPrivateKey**)&pPriKey);
        AutoPtr<IRSAPrivateKey> rsaKey = IRSAPrivateKey::Probe(pPriKey);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> privateExponent;
        IRSAPrivateKey::Probe(rsaKey)->GetPrivateExponent((IBigInteger**)&privateExponent);
        AutoPtr<IRSAPrivateKeySpec> p;
        CRSAPrivateKeySpec::New(modulus, privateExponent, (IRSAPrivateKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else if (IPrivateKey::Probe(key) != NULL
            && keySpec == EIID_IPKCS8EncodedKeySpec) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String fm;
        key->GetFormat(&fm);
        if (!fm.Equals("PKCS#8")) {
            // throw new InvalidKeySpecException("Encoding type must be PKCS#8; was "
            //         + key.getFormat());
            return NOERROR;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return NOERROR;
        }
        AutoPtr<IPKCS8EncodedKeySpec> p;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else if (IPublicKey::Probe(key) != NULL && keySpec == EIID_IX509EncodedKeySpec) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String fm;
        key->GetFormat(&fm);
        if (!fm.Equals("X.509")) {
            // throw new InvalidKeySpecException("Encoding type must be X.509; was "
            //         + key.getFormat());
            return NOERROR;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return NOERROR;
        }
        AutoPtr<IX509EncodedKeySpec> p;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&p);
        *retkeySpec = IKeySpec::Probe(p);
        REFCOUNT_ADD(*retkeySpec)
        return NOERROR;
    }
    else {
        // throw new InvalidKeySpecException("Unsupported key type and key spec combination; key="
        //         + key.getClass().getName() + ", keySpec=" + keySpec.getName());
        return NOERROR;
    }
}

ECode OpenSSLRSAKeyFactory::EngineTranslateKey(
    /* [in] */ IKey* key,
    /* [out] */ IKey** translatedKey)
{
    VALIDATE_NOT_NULL(translatedKey)
    if (key == NULL) {
        // throw new InvalidKeyException("key == null");
        return NOERROR;
    }

    String fm;
    key->GetFormat(&fm);
    if ((IOpenSSLRSAPublicKey::Probe(key) != NULL) || (IOpenSSLRSAPrivateKey::Probe(key) != NULL)) {
        *translatedKey = key;
        REFCOUNT_ADD(*translatedKey)
        return NOERROR;
    }
    else if (IRSAPublicKey::Probe(key) != NULL) {
        AutoPtr<IRSAPublicKey> rsaKey = IRSAPublicKey::Probe(key);

        // try {
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> publicExponent;
        rsaKey->GetPublicExponent((IBigInteger**)&publicExponent);
        AutoPtr<IRSAPublicKeySpec> pKeySpec;
        CRSAPublicKeySpec::New(modulus,
                    publicExponent, (IRSAPublicKeySpec**)&pKeySpec);
        AutoPtr<IPublicKey> pk;
        EngineGeneratePublic(IKeySpec::Probe(pKeySpec), (IPublicKey**)&pk);
        *translatedKey = IKey::Probe(pk);
        REFCOUNT_ADD(*translatedKey)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if (IRSAPrivateCrtKey::Probe(key) != NULL) {
        AutoPtr<IRSAPrivateCrtKey> rsaKey = IRSAPrivateCrtKey::Probe(key);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> publicExponent;
        rsaKey->GetPublicExponent((IBigInteger**)&publicExponent);
        AutoPtr<IBigInteger> privateExponent;
        IRSAPrivateKey::Probe(rsaKey)->GetPrivateExponent((IBigInteger**)&privateExponent);
        AutoPtr<IBigInteger> primeP;
        rsaKey->GetPrimeP((IBigInteger**)&primeP);
        AutoPtr<IBigInteger> primeQ;
        rsaKey->GetPrimeQ((IBigInteger**)&primeQ);
        AutoPtr<IBigInteger> primeExponentP;
        rsaKey->GetPrimeExponentP((IBigInteger**)&primeExponentP);
        AutoPtr<IBigInteger> primeExponentQ;
        rsaKey->GetPrimeExponentQ((IBigInteger**)&primeExponentQ);
        AutoPtr<IBigInteger> crtCoefficient;
        rsaKey->GetCrtCoefficient((IBigInteger**)&crtCoefficient);

        // try {
        AutoPtr<IRSAPrivateCrtKeySpec> pKeySpec;
        CRSAPrivateCrtKeySpec::New(modulus, publicExponent,
                    privateExponent, primeP, primeQ, primeExponentP, primeExponentQ,
                    crtCoefficient, (IRSAPrivateCrtKeySpec**)&pKeySpec);
        AutoPtr<IPrivateKey> pk;
        EngineGeneratePrivate(IKeySpec::Probe(pKeySpec), (IPrivateKey**)&pk);
        *translatedKey = IKey::Probe(pk);
        REFCOUNT_ADD(*translatedKey)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if (IRSAPrivateKey::Probe(key) != NULL) {
        AutoPtr<IRSAPrivateKey> rsaKey = IRSAPrivateKey::Probe(key);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaKey)->GetModulus((IBigInteger**)&modulus);
        AutoPtr<IBigInteger> privateExponent;
        IRSAPrivateKey::Probe(rsaKey)->GetPrivateExponent((IBigInteger**)&privateExponent);

        // try {
        AutoPtr<IRSAPrivateKeySpec> pKeySpec;
        CRSAPrivateKeySpec::New(modulus, privateExponent, (IRSAPrivateKeySpec**)&pKeySpec);
        AutoPtr<IPrivateKey> pk;
        EngineGeneratePrivate(IKeySpec::Probe(pKeySpec), (IPrivateKey**)&pk);
        *translatedKey = IKey::Probe(pk);
        REFCOUNT_ADD(*translatedKey)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if ((IPrivateKey::Probe(key) != NULL) && (fm.Equals("PKCS#8"))) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        if (encoded == NULL) {
            // throw new InvalidKeyException("Key does not support encoding");
            return NOERROR;
        }
        // try {
        AutoPtr<IPKCS8EncodedKeySpec> pKeySpec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&pKeySpec);
        AutoPtr<IPrivateKey> pk;
        EngineGeneratePrivate(IKeySpec::Probe(pKeySpec), (IPrivateKey**)&pk);
        *translatedKey = IKey::Probe(pk);
        REFCOUNT_ADD(*translatedKey)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if ((IPublicKey::Probe(key) != NULL) && (fm.Equals("X.509"))) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        if (encoded == NULL) {
            // throw new InvalidKeyException("Key does not support encoding");
            return NOERROR;
        }
        // try {
        AutoPtr<IX509EncodedKeySpec> pKeySpec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&pKeySpec);
        AutoPtr<IPublicKey> pk;
        EngineGeneratePublic(IKeySpec::Probe(pKeySpec), (IPublicKey**)&pk);
        *translatedKey = IKey::Probe(pk);
        REFCOUNT_ADD(*translatedKey)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else {
        // throw new InvalidKeyException("Key must be an RSA public or private key; was "
        //         + key.getClass().getName());
        return NOERROR;
    }
}

} // namespace Conscrypt
} // namespace Org
