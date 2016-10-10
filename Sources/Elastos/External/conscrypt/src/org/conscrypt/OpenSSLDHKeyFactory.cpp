#include "org/conscrypt/OpenSSLDHKeyFactory.h"
#include "org/conscrypt/OpenSSLDHPrivateKey.h"
#include "org/conscrypt/OpenSSLDHPublicKey.h"
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastosx::Crypto::Interfaces::IDHKey;
using Elastosx::Crypto::Spec::CDHPrivateKeySpec;
using Elastosx::Crypto::Spec::CDHPublicKeySpec;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLDHKeyFactory, Object, IOpenSSLDHKeyFactory)
// TODO: Need KeyFactorySpi
// CAR_INTERFACE_IMPL(OpenSSLDHKeyFactory, KeyFactorySpi, IOpenSSLDHKeyFactory)

ECode OpenSSLDHKeyFactory::EngineGeneratePublic(
   /* [in] */ IKeySpec* keySpec,
   /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    if (IDHPublicKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<OpenSSLDHPublicKey> key = new OpenSSLDHPublicKey();
        key->constructor(IDHPublicKeySpec::Probe(keySpec));
        *result = IPublicKey::Probe(key);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IX509EncodedKeySpec::Probe(keySpec) != NULL) {
        *result = OpenSSLKey::GetPublicKey(IX509EncodedKeySpec::Probe(keySpec),
                INativeCrypto::EVP_PKEY_DH);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    // throw new InvalidKeySpecException("Must use DHPublicKeySpec or X509EncodedKeySpec; was "
    //          + keySpec.getClass().getName());
    return E_INVALID_KEY_SPEC_EXCEPTION;
}

ECode OpenSSLDHKeyFactory::EngineGeneratePrivate(
    /* [in] */ IKeySpec* keySpec,
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    if (IDHPrivateKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<OpenSSLDHPrivateKey> key = new OpenSSLDHPrivateKey();
        key->constructor(IDHPrivateKeySpec::Probe(keySpec));
        *result = IPrivateKey::Probe(key);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IPKCS8EncodedKeySpec::Probe(keySpec) != NULL) {
        *result = OpenSSLKey::GetPrivateKey(IPKCS8EncodedKeySpec::Probe(keySpec),
                INativeCrypto::EVP_PKEY_DH);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    // throw new InvalidKeySpecException("Must use DHPrivateKeySpec or PKCS8EncodedKeySpec; was "
    //          + keySpec.getClass().getName());
    return E_INVALID_KEY_SPEC_EXCEPTION;
}

ECode OpenSSLDHKeyFactory::EngineGetKeySpec(
    /* [in] */ IKey* key,
    /* [in] */ ClassID keySpec,
    /* [out] */ IKeySpec** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (key == NULL) {
        // throw new InvalidKeySpecException("key == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

// TODO:
    // if (keySpec == NULL) {
    //     // throw new InvalidKeySpecException("keySpec == NULL");
    //     return E_INVALID_KEY_SPEC_EXCEPTION;
    // }

    String str;
    key->GetAlgorithm(&str);
    if (!str.Equals("DH")) {
        // throw new InvalidKeySpecException("Key must be a DH key");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    assert(0 && "TODO");
    if ((IDHPublicKey::Probe(key) != NULL)
            /*TODO: && DHPublicKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<IDHPublicKey> dhKey = IDHPublicKey::Probe(key);
        AutoPtr<IDHParameterSpec> params;
        IDHKey::Probe(dhKey)->GetParams((IDHParameterSpec**)&params);
        AutoPtr<IBigInteger> bY;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bG;
        dhKey->GetY((IBigInteger**)&bY);
        params->GetP((IBigInteger**)&bP);
        params->GetG((IBigInteger**)&bG);
        return CDHPublicKeySpec::New(bY, bP, bG, result);
    }
    else if ((IPublicKey::Probe(key) != NULL)
            /*TODO: && DHPublicKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("X.509") || encoded == NULL) {
            // throw new InvalidKeySpecException("Not a valid X.509 encoding");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        AutoPtr<IX509EncodedKeySpec> spec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&spec);
        AutoPtr<IPublicKey> pk;
        EngineGeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&pk);
        AutoPtr<IDHPublicKey> dhKey = IDHPublicKey::Probe(pk);

        AutoPtr<IDHParameterSpec> params;
        IDHKey::Probe(dhKey)->GetParams((IDHParameterSpec**)&params);
        AutoPtr<IBigInteger> bY;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bG;
        dhKey->GetY((IBigInteger**)&bY);
        params->GetP((IBigInteger**)&bP);
        params->GetG((IBigInteger**)&bG);
        return CDHPublicKeySpec::New(bY, bP, bG, result);
    }
    else if ((IDHPrivateKey::Probe(key) != NULL)
            /*TODO: && DHPrivateKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<IDHPrivateKey> dhKey = IDHPrivateKey::Probe(key);
        AutoPtr<IDHParameterSpec> params;
        IDHKey::Probe(dhKey)->GetParams((IDHParameterSpec**)&params);
        AutoPtr<IBigInteger> bX;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bG;
        dhKey->GetX((IBigInteger**)&bX);
        params->GetP((IBigInteger**)&bP);
        params->GetG((IBigInteger**)&bG);
        return CDHPrivateKeySpec::New(bX, bP, bG, result);
    }
    else if ((IPrivateKey::Probe(key) != NULL)
            /*TODO: && DHPrivateKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("PKCS#8") || encoded == NULL) {
            // throw new InvalidKeySpecException("Not a valid PKCS#8 encoding");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }

        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> pk;
        EngineGeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&pk);
        AutoPtr<IDHPrivateKey> dhKey = IDHPrivateKey::Probe(pk);
        AutoPtr<IDHParameterSpec> params;
        IDHKey::Probe(dhKey)->GetParams((IDHParameterSpec**)&params);
        AutoPtr<IBigInteger> bX;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bG;
        dhKey->GetX((IBigInteger**)&bX);
        params->GetP((IBigInteger**)&bP);
        params->GetG((IBigInteger**)&bG);
        return CDHPrivateKeySpec::New(bX, bP, bG, result);
    }
    else if ((IPrivateKey::Probe(key) != NULL)
            /*TODO: && PKCS8EncodedKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("PKCS#8")) {
            // throw new InvalidKeySpecException("Encoding type must be PKCS#8; was "
            //          + key.getFormat());
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        return CPKCS8EncodedKeySpec::New(encoded, result);
    }
    else if ((IPublicKey::Probe(key) != NULL)
            /*TODO: && X509EncodedKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("X.509")) {
            // throw new InvalidKeySpecException("Encoding type must be X.509; was "
            //          + key.getFormat());
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        return CX509EncodedKeySpec::New(encoded, result);
    }
    else {
        // throw new InvalidKeySpecException("Unsupported key type and key spec combination; key="
        //          + key.getClass().getName() + ", keySpec=" + keySpec.getName());
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLDHKeyFactory::EngineTranslateKey(
    /* [in] */ IKey* key,
    /* [out] */ IKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }

    String format;
    key->GetFormat(&format);
    if ((IOpenSSLDHPublicKey::Probe(key) != NULL)
            || (IOpenSSLDHPrivateKey::Probe(key) != NULL)) {
        *result = key;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IDHPublicKey::Probe(key) != NULL) {
        AutoPtr<IDHPublicKey> dhKey = IDHPublicKey::Probe(key);

        AutoPtr<IBigInteger> y;
        dhKey->GetY((IBigInteger**)&y);

        AutoPtr<IDHParameterSpec> params;
        IDHKey::Probe(dhKey)->GetParams((IDHParameterSpec**)&params);
        AutoPtr<IBigInteger> p;
        params->GetP((IBigInteger**)&p);
        AutoPtr<IBigInteger> g;
        params->GetG((IBigInteger**)&g);

        // try {
        AutoPtr<IDHPublicKeySpec> spec;
        CDHPublicKeySpec::New(y, p, g, (IDHPublicKeySpec**)&spec);
        AutoPtr<IPublicKey> publicKey;
        EngineGeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&publicKey);
        *result = IKey::Probe(publicKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if (IDHPrivateKey::Probe(key) != NULL) {
        AutoPtr<IDHPrivateKey> dhKey = IDHPrivateKey::Probe(key);

        AutoPtr<IBigInteger> x;
        dhKey->GetX((IBigInteger**)&x);

        AutoPtr<IDHParameterSpec> params;
        IDHKey::Probe(dhKey)->GetParams((IDHParameterSpec**)&params);
        AutoPtr<IBigInteger> p;
        params->GetP((IBigInteger**)&p);
        AutoPtr<IBigInteger> g;
        params->GetG((IBigInteger**)&g);

        // try {
        AutoPtr<IDHPrivateKeySpec> spec;
        CDHPrivateKeySpec::New(x, p, g, (IDHPrivateKeySpec**)&spec);
        AutoPtr<IPrivateKey> privateKey;
        EngineGeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privateKey);
        *result = IKey::Probe(privateKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if ((IPrivateKey::Probe(key) != NULL) && (format.Equals("PKCS#8"))) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        if (encoded == NULL) {
            // throw new InvalidKeyException("Key does not support encoding");
            return E_INVALID_KEY_EXCEPTION;
        }
        // try {
        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> privateKey;
        EngineGeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privateKey);
        *result = IKey::Probe(privateKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if ((IPublicKey::Probe(key) != NULL) && (format.Equals("X.509"))) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        if (encoded == NULL) {
            // throw new InvalidKeyException("Key does not support encoding");
            return E_INVALID_KEY_EXCEPTION;
        }
        // try {
        AutoPtr<IX509EncodedKeySpec> spec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&spec);
        AutoPtr<IPublicKey> publicKey;
        EngineGeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&publicKey);
        *result = IKey::Probe(publicKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else {
        // throw new InvalidKeyException("Key must be DH public or private key; was "
        //          + key.getClass().getName());
        return E_INVALID_KEY_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
