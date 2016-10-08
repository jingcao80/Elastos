#include "org/conscrypt/OpenSSLDSAKeyFactory.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLDSAPrivateKey.h"
#include "org/conscrypt/OpenSSLDSAPublicKey.h"
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::Security::Spec::CDSAPrivateKeySpec;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastosx::Crypto::Spec::CDSAPublicKeySpec;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLDSAKeyFactory, KeyFactorySpi, IOpenSSLDSAKeyFactory)

ECode OpenSSLDSAKeyFactory::EngineGeneratePublic(
   /* [in] */ IKeySpec* keySpec,
   /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    if (IDSAPublicKeySpec::Probe(keySpec) != NULL) {
        *result = new OpenSSLDSAPublicKey(IDSAPublicKeySpec::Probe(keySpec));
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IX509EncodedKeySpec::Probe(keySpec) != NULL) {
        return OpenSSLKey::GetPublicKey(IX509EncodedKeySpec::Probe(keySpec), INativeCrypto::EVP_PKEY_DSA, result);
    }
    // throw new InvalidKeySpecException("Must use DSAPublicKeySpec or X509EncodedKeySpec; was "
    //         + keySpec.getClass().getName());
    return E_INVALID_KEY_SPEC_EXCEPTION;
}

ECode OpenSSLDSAKeyFactory::EngineGeneratePrivate(
    /* [in] */ IKeySpec* keySpec,
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    if (IDSAPrivateKeySpec::Probe(keySpec) != NULL) {
        *result = new OpenSSLDSAPrivateKey(IDSAPrivateKeySpec::Probe(keySpec));
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IPKCS8EncodedKeySpec::Probe(keySpec) != NULL) {
        return OpenSSLKey::GetPrivateKey(IPKCS8EncodedKeySpec::Probe(keySpec), INativeCrypto::EVP_PKEY_DSA, result);
    }
    // throw new InvalidKeySpecException("Must use DSAPrivateKeySpec or PKCS8EncodedKeySpec; was "
    //         + keySpec.getClass().getName());
    return E_INVALID_KEY_SPEC_EXCEPTION;
}

ECode OpenSSLDSAKeyFactory::EngineGetKeySpec(
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

    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    String str;
    key->GetAlgorithm(&str);
    if (!str.Equals("DSA")) {
        // throw new InvalidKeySpecException("Key must be a DSA key");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    if ((IDSAPublicKey::Probe(key) != NULL)  && DSAPublicKeySpec.class.isAssignableFrom(keySpec)) {
        AutoPtr<IDSAPublicKey> dsaKey = IDSAPublicKey::Probe(key);
        AutoPtr<DSAParams> params;
        dsaKey->GetParams((DSAParams**)&params);
        AutoPtr<IBigInteger> bY;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bQ;
        AutoPtr<IBigInteger> bG;
        dsaKey->GetY((IBigInteger**)&bY);
        params->GetP((IBigInteger**)&bP);
        params->GetQ((IBigInteger**)&bQ);
        params->GetG((IBigInteger**)&bG);
        return CDSAPublicKeySpec::New(bY, bP, bQ, bG, result);
    }
    else if ((IPublicKey::Probe(key) != NULL) && DSAPublicKeySpec.class.isAssignableFrom(keySpec)) {
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
        AutoPtr<IIPublicKey> pk;
        EngineGeneratePublic(spec, (IIPublicKey**)&pk);

        AutoPtr<IDSAPublicKey> dsaKey = IDSAPublicKey::Probe(pk);
        AutoPtr<DSAParams> params;
        dsaKey->GetParams((DSAParams**)&params);
        AutoPtr<IBigInteger> bY;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bQ;
        AutoPtr<IBigInteger> bG;
        dsaKey->GetY((IBigInteger**)&bY);
        params->GetP((IBigInteger**)&bP);
        params->GetQ((IBigInteger**)&bQ);
        params->GetG((IBigInteger**)&bG);
        return CDSAPublicKeySpec::New(bY, bP, bQ, bG, result);
    }
    else if ((IDSAPrivateKey::Probe(key) != NULL) && DSAPrivateKeySpec.class.isAssignableFrom(keySpec)) {
        AutoPtr<IDSAPrivateKey> dsaKey = IDSAPrivateKey::Probe(key);
        AutoPtr<IDSAParams> params;
        dsaKey->GetParams((IDSAParams**)&prarms);
        AutoPtr<IBigInteger> bY;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bQ;
        AutoPtr<IBigInteger> bG;
        dsaKey->GetY((IBigInteger**)&bY);
        params->GetP((IBigInteger**)&bP);
        params->GetQ((IBigInteger**)&bQ);
        params->GetG((IBigInteger**)&bG);
        return CDSAPrivateKeySpec::New(bX, bP, bQ, bG, result);
    }
    else if ((IPrivateKey::Probe(key) != NULL) && DSAPrivateKeySpec.class.isAssignableFrom(keySpec)) {
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
        AutoPtr<IIPublicKey> pk;
        EngineGeneratePrivate(spec, (IIPublicKey**)&pk);
        AutoPtr<IDSAPrivateKey> dsaKey = IDSAPrivateKey::Probe(pk);
        AutoPtr<IDSAParams> params;
        dsaKey->GetParams((IDSAParams**)&params);
        AutoPtr<IBigInteger> bY;
        AutoPtr<IBigInteger> bP;
        AutoPtr<IBigInteger> bQ;
        AutoPtr<IBigInteger> bG;
        dsaKey->GetY((IBigInteger**)&bY);
        params->GetP((IBigInteger**)&bP);
        params->GetQ((IBigInteger**)&bQ);
        params->GetG((IBigInteger**)&bG);
        return CDSAPrivateKeySpec::New(bX, bP, bQ, bG, result);
    }
    else if ((IPrivateKey::Probe(key) != NULL)
            && PKCS8EncodedKeySpec.class.isAssignableFrom(keySpec)) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("PKCS#8")) {
            // throw new InvalidKeySpecException("Encoding type must be PKCS#8; was " + key.getFormat());
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        return CPKCS8EncodedKeySpec::New(encoded, result);
    }
    else if ((IPublicKey::Probe(key) != NULL) && X509EncodedKeySpec.class.isAssignableFrom(keySpec)) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("X.509")) {
            // throw new InvalidKeySpecException("Encoding type must be X.509; was " + key.getFormat());
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        return CX509EncodedKeySpec::New(encoded, result);
    }
    else {
        // throw new InvalidKeySpecException("Unsupported key type and key spec combination; key=" + key.getClass().getName() + ", keySpec=" + keySpec.getName());
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLDSAKeyFactory::EngineTranslateKey(
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
    if ((IOpenSSLDSAPublicKey::Probe(key) != NULL) || (IOpenSSLDSAPrivateKey::Probe(key) != NULL)) {
        *result = key;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IDSAPublicKey::Probe(key) != NULL) {
        AutoPtr<IDSAPublicKey> dsaKey = IDSAPublicKey::Probe(key);

        AutoPtr<IBigInteger> y;
        dsaKey->GetY((IBigInteger**)&y);

        AutoPtr<IDSAParams> params;
        dsaKey->GetParams((IDSAParams**)&params);
        AutoPtr<IBigInteger> p;
        params->GetP((IBigInteger**)&p);
        AutoPtr<IBigInteger> q;
        params->GetG((IBigInteger**)&q);
        AutoPtr<IBigInteger> g;
        params->GetG((IBigInteger**)&g);

        // try {
        AutoPtr<IDSAPublicKeySpec> spec;
        CDSAPublicKeySpec::New(y, p, q, g, (IDSAPublicKeySpec**)&spec);
        return EngineGeneratePublic(spec, result);
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else if (IDSAPrivateKey::Probe(key) != NULL) {
        AutoPtr<IDSAPrivateKey> dsaKey = IDSAPrivateKey::Probe(key);

        AutoPtr<IBigInteger> x;
        dsaKey->GetX((IBigInteger**)&x);

        AutoPtr<IDSAParams> params;
        dsaKey->GetParams((IDSAParams**)&params);
        AutoPtr<IBigInteger> p;
        params->GetP((IBigInteger**)&p);
        AutoPtr<IBigInteger> q;
        params->GetP((IBigInteger**)&q);
        AutoPtr<IBigInteger> g;
        params->GetG((IBigInteger**)&g);

        // try {
        AutoPtr<IDSAPrivateKeySpec> spec;
        CDSAPrivateKeySpec::New(x, p, q, g, (IDSAPrivateKeySpec**)&spec);
        return EngineGeneratePrivate(spec, result);
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
        return EngineGeneratePrivate(spec, result);
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
        return EngineGeneratePublic(spec, result);
        // } catch (InvalidKeySpecException e) {
        //     throw new InvalidKeyException(e);
        // }
    }
    else {
        // throw new InvalidKeyException("Key must be DSA public or private key; was " + key.getClass().getName());
        return E_INVALID_KEY_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
