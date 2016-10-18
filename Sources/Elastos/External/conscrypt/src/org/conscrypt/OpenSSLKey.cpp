
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLKey.h"
// #include "OpenSSLRSAPrivateKey.h"
// #include "OpenSSLDSAPrivateKey.h"
// #include "OpenSSLECPrivateKey.h"
#include "NativeCrypto.h"
#include "COpenSSLKey.h"
#include "COpenSSLRSAPublicKey.h"
#include "COpenSSLECPublicKey.h"
#include "COpenSSLRSAPrivateKey.h"
#include "COpenSSLECPrivateKey.h"

using Elastos::Security::IKey;
using Elastos::Security::Spec::IEncodedKeySpec;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLKey::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLKey, Object, IOpenSSLKey)

ECode OpenSSLKey::constructor(
    /* [in] */ Int64 ctx)
{
    mCtx = ctx;
    mEngine = NULL;
    mAlias = NULL;
    return NOERROR;
}

ECode OpenSSLKey::constructor(
    /* [in] */ Int64 ctx,
    /* [in] */ IOpenSSLEngine* engine,
    /* [in] */ const String& alias)
{
    mCtx = ctx;
    mEngine = engine;
    mAlias = alias;
    return NOERROR;
}

ECode OpenSSLKey::GetPkeyContext(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCtx;
    return NOERROR;
}

ECode OpenSSLKey::GetEngine(
    /* [out] */ IOpenSSLEngine** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEngine;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLKey::IsEngineBased(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEngine != NULL;
    return NOERROR;
}

ECode OpenSSLKey::GetAlias(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAlias;
    return NOERROR;
}

AutoPtr<IOpenSSLKey> OpenSSLKey::FromPrivateKey(
    /* [in] */ IPrivateKey* key)
{
    if (IOpenSSLKeyHolder::Probe(key) != NULL) {
        AutoPtr<IOpenSSLKey> res;
        IOpenSSLKeyHolder::Probe(key)->GetOpenSSLKey((IOpenSSLKey**)&res);
        return res;
    }

    String keyFormat;
    IKey::Probe(key)->GetFormat(&keyFormat);
    if (keyFormat.IsNull()) {
        return WrapPrivateKey(key);
    }
    else if (!keyFormat.Equals("PKCS#8")) {
        // throw new InvalidKeyException("Unknown key format " + keyFormat);
        return NULL;
    }

    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(key)->GetEncoded((ArrayOf<Byte>**)&encoded);
    if (encoded == NULL) {
        // throw new InvalidKeyException("Key encoding is null");
        return NULL;
    }

    Int64 key_info = 0;
    NativeCrypto::D2i_PKCS8_PRIV_KEY_INFO(encoded, &key_info);
    AutoPtr<IOpenSSLKey> res;
    COpenSSLKey::New(key_info, (IOpenSSLKey**)&res);
    return res;
}

AutoPtr<IOpenSSLKey> OpenSSLKey::WrapPrivateKey(
    /* [in] */ IPrivateKey* key)
{
    // if (IRSAPrivateKey::Probe(key) != NULL) {
    //     return OpenSSLRSAPrivateKey::WrapPlatformKey(IRSAPrivateKey::Probe(key));
    // }
    // else if (IDSAPrivateKey::Probe(key) != NULL) {
    //     return OpenSSLDSAPrivateKey::WrapPlatformKey(IDSAPrivateKey::Probe(key));
    // }
    // else if (IECPrivateKey::Probe(key) != NULL) {
    //     return OpenSSLECPrivateKey::WrapPlatformKey(IECPrivateKey::Probe(key));
    // }
    // else {
    //     // throw new InvalidKeyException("Unknown key type: " + key.toString());
    //     return NULL;
    // }
    assert(0 && "TODO");
    return NULL;
}

AutoPtr<IOpenSSLKey> OpenSSLKey::FromPublicKey(
    /* [in] */ IPublicKey* key)
{
    if (IOpenSSLKeyHolder::Probe(key) != NULL) {
        AutoPtr<IOpenSSLKey> res;
        IOpenSSLKeyHolder::Probe(key)->GetOpenSSLKey((IOpenSSLKey**)&res);
        return res;
    }

    String fm;
    IKey::Probe(key)->GetFormat(&fm);
    if (!fm.Equals("X.509")) {
        // throw new InvalidKeyException("Unknown key format " + key.getFormat());
        return NULL;
    }

    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(key)->GetEncoded((ArrayOf<Byte>**)&encoded);
    if (encoded == NULL) {
        // throw new InvalidKeyException("Key encoding is null");
        return NULL;
    }

    Int64 pubkey = 0;
    NativeCrypto::D2i_PUBKEY(encoded, &pubkey);
    AutoPtr<IOpenSSLKey> res;
    COpenSSLKey::New(pubkey, (IOpenSSLKey**)&res);
    return res;
}

ECode OpenSSLKey::GetPublicKey(
    /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    NativeCrypto::EVP_PKEY_type(mCtx, &type);
    switch (type) {
        case INativeCrypto::EVP_PKEY_RSA: {
            AutoPtr<IOpenSSLRSAPublicKey> p;
            COpenSSLRSAPublicKey::New(this, (IOpenSSLRSAPublicKey**)&p);
            *result = IPublicKey::Probe(p);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case INativeCrypto::EVP_PKEY_DH: {
            AutoPtr<IOpenSSLDHPublicKey> p;
            assert(0 && "TODO");
            // = new OpenSSLDHPublicKey(this);
            *result = IPublicKey::Probe(p);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case INativeCrypto::EVP_PKEY_DSA: {
            AutoPtr<IOpenSSLDSAPublicKey> p;
            assert(0 && "TODO");
            // = new OpenSSLDSAPublicKey(this);
            *result = IPublicKey::Probe(p);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case INativeCrypto::EVP_PKEY_EC: {
            AutoPtr<IOpenSSLECPublicKey> p;
            COpenSSLECPublicKey::New(this, (IOpenSSLECPublicKey**)&p);
            *result = IPublicKey::Probe(p);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        default: {
            // throw new NoSuchAlgorithmException("unknown PKEY type");
            return NOERROR;
        }
    }
    return NOERROR;
}

AutoPtr<IPublicKey> OpenSSLKey::GetPublicKey(
    /* [in] */ IX509EncodedKeySpec* keySpec,
    /* [in] */ Int32 type)
{
    AutoPtr<IX509EncodedKeySpec> x509KeySpec = keySpec;

    AutoPtr<IOpenSSLKey> key;
    AutoPtr<ArrayOf<Byte> > encoded;
    IEncodedKeySpec::Probe(x509KeySpec)->GetEncoded((ArrayOf<Byte>**)&encoded);
    Int64 pubKey = 0;
    NativeCrypto::D2i_PUBKEY(encoded, &pubKey);
    COpenSSLKey::New(pubKey, (IOpenSSLKey**)&key);
    Int64 keyContext = 0;
    key->GetPkeyContext(&keyContext);
    Int32 keyType = 0;
    NativeCrypto::EVP_PKEY_type(keyContext, &keyType);
    if (keyType != type) {
        // throw new InvalidKeySpecException("Unexpected key type");
        return NULL;
    }

    AutoPtr<IPublicKey> res;
    key->GetPublicKey((IPublicKey**)&res);
    return res;
}

ECode OpenSSLKey::GetPrivateKey(
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    NativeCrypto::EVP_PKEY_type(mCtx, &type);
    switch (type) {
        case INativeCrypto::EVP_PKEY_RSA: {
            AutoPtr<IOpenSSLRSAPrivateKey> p;
            COpenSSLRSAPrivateKey::New(this, (IOpenSSLRSAPrivateKey**)&p);
            *result = IPrivateKey::Probe(p);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case INativeCrypto::EVP_PKEY_DH: {
            AutoPtr<IOpenSSLDHPrivateKey> p;
            assert(0 && "TODO");
            // = new OpenSSLDHPrivateKey(this);
            *result = IPrivateKey::Probe(p);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case INativeCrypto::EVP_PKEY_DSA: {
            assert(0 && "TODO");
            // AutoPtr<IOpenSSLDSAPrivateKey> p;
            // = new OpenSSLDSAPrivateKey(this);
            // *result = IPrivateKey::Probe(p);
            // REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case INativeCrypto::EVP_PKEY_EC: {
            AutoPtr<IOpenSSLECPrivateKey> p;
            COpenSSLECPrivateKey::New(this, (IOpenSSLECPrivateKey**)&p);
            *result = IPrivateKey::Probe(p);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        default: {
            // throw new NoSuchAlgorithmException("unknown PKEY type");
            return NOERROR;
        }
    }
    return NOERROR;
}

AutoPtr<IPrivateKey> OpenSSLKey::GetPrivateKey(
    /* [in] */ IPKCS8EncodedKeySpec* keySpec,
    /* [in] */ Int32 type)
{
    AutoPtr<IPKCS8EncodedKeySpec> pkcs8KeySpec = keySpec;

    AutoPtr<IOpenSSLKey> key;
    AutoPtr<ArrayOf<Byte> > encoded;
    IEncodedKeySpec::Probe(pkcs8KeySpec)->GetEncoded((ArrayOf<Byte>**)&encoded);
    Int64 key_info = 0;
    NativeCrypto::D2i_PKCS8_PRIV_KEY_INFO(encoded, &key_info);
    COpenSSLKey::New(key_info, (IOpenSSLKey**)&key);
    Int64 keyContext = 0;
    key->GetPkeyContext(&keyContext);
    Int32 keyType = 0;
    NativeCrypto::EVP_PKEY_type(keyContext, &keyType);
    if (keyType != type) {
        // throw new InvalidKeySpecException("Unexpected key type");
        return NULL;
    }

    AutoPtr<IPrivateKey> priKey;
    key->GetPrivateKey((IPrivateKey**)&priKey);
    return priKey;
}

ECode OpenSSLKey::GetSecretKey(
    /* [in] */ const String& algorithm,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 keyType = 0;
    NativeCrypto::EVP_PKEY_type(mCtx, &keyType);
    switch (keyType) {
        case INativeCrypto::EVP_PKEY_HMAC:
        case INativeCrypto::EVP_PKEY_CMAC: {
            assert(0 && "TODO");
            // AutoPtr<IOpenSSLSecretKey> p;
            // = new OpenSSLSecretKey(algorithm, this);
            // *result = ISecretKey::Probe(p);
            // REFCOUNT_ADD(*result)
            return NOERROR;
        }
        default: {
            // throw new NoSuchAlgorithmException("unknown PKEY type");
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode OpenSSLKey::Finalize()
{
    if (mCtx != 0) {
        NativeCrypto::EVP_PKEY_free(mCtx);
    }
    Finalize();
    return NOERROR;
}

ECode OpenSSLKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (o == this->Probe(EIID_IInterface)) {
        *result = TRUE;
        return NOERROR;
    }

    if (!(IOpenSSLKey::Probe(o) != NULL)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IOpenSSLKey> other = IOpenSSLKey::Probe(o);
    Int64 keyContext = 0;
    other->GetPkeyContext(&keyContext);
    if (mCtx == keyContext) {
        *result = TRUE;
        return NOERROR;
    }

    /*
     * ENGINE-based keys must be checked in a special way.
     */
    AutoPtr<IOpenSSLEngine> otherEngine;
    other->GetEngine((IOpenSSLEngine**)&otherEngine);
    Boolean bEquals = FALSE;
    assert(0 && "TODO");
    if (mEngine == NULL) {
        if (otherEngine != NULL) {
            *result = FALSE;
            return NOERROR;
        }
    }
    // else if (!(mEngine->Equals(otherEngine, &bEquals), bEquals)) {
    //     *result = FALSE;
    //     return NOERROR;
    // }
    else {
        String otherAlias;
        other->GetAlias(&otherAlias);
        if (mAlias != NULL) {
            *result = mAlias.Equals(otherAlias);
            return NOERROR;
        }
        else if (!otherAlias.IsNull()) {
            *result = FALSE;
            return NOERROR;
        }
    }

    Int32 keyCmp = 0;
    NativeCrypto::EVP_PKEY_cmp(mCtx, keyContext, &keyCmp);
    *result = keyCmp == 1;
    return NOERROR;
}

ECode OpenSSLKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 hash = 1;
    hash = hash * 17 + (Int32) mCtx;
    assert(0 && "TODO");
    // hash = hash * 31 + (Int32) (mEngine == NULL ? 0 : mEngine->GetEngineContext());
    *result = hash;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
