
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLX509V3CertificateGenerator.h"
#include "NativeCrypto.h"
#include "COpenSSLX509Certificate.h"
#include "COpenSSLKey.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Security::IKey;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

/* subjectName: ',' seperated value, such as C=CN, L=SH, ST=SH, CN=kortide, O=kortide, OU=OS*/
ECode OpenSSLX509V3CertificateGenerator::Generate(
    /* [in] */ IPublicKey *pubkey,
    /* [in] */ IPrivateKey *privkey,
    /* [in] */ IBigInteger* serialNumber,
    /* [in] */ IDate* start,
    /* [in] */ IDate* end,
    /* [in] */ const String& subjectName,
    /* [in] */ const String& _hashalg,
    /* [out] */ IX509Certificate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IOpenSSLKey> publicKey = FromPublicKey(pubkey);
    AutoPtr<IOpenSSLKey> privateKey = FromPrivateKey(privkey);

    String hashalg = _hashalg;
    Int32 pkeyType = 0;
    Int64 pubkeycontext = 0;
    privateKey->GetPkeyContext(&pubkeycontext);
    NativeCrypto::EVP_PKEY_type(pubkeycontext, &pkeyType);

    Int32 pkeyType2 = 0;
    Int64 privkeycontext = 0;
    publicKey->GetPkeyContext(&privkeycontext);
    NativeCrypto::EVP_PKEY_type(privkeycontext, &pkeyType2);
    assert(pkeyType2 == pkeyType && "keytype in public key and private key should be same");

    //if (hashalg.IsNull()) // Note: now the hashalg depends on the Key generate algorithm
    {
        switch (pkeyType) {
            case INativeCrypto::EVP_PKEY_RSA:
                hashalg = String("sha256");
                break;
            case INativeCrypto::EVP_PKEY_DSA:
                hashalg = String("sha1");
                break;
            case INativeCrypto::EVP_PKEY_EC:
                hashalg = String("sha256");
                break;
            default:
                Logger::E("OpenSSLX509V3CertificateGenerator", "keyType is wrong, should be one of RSA/DSA/EC, pkeyType:%d", pkeyType);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    Int64 certCtx = 0;
    NativeCrypto::Make_Self_Signed_X509(
            pubkeycontext, privkeycontext, serialNumber, start, end,
            subjectName, subjectName,hashalg, &certCtx);
    if (certCtx == 0l) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IOpenSSLX509Certificate> res;
    COpenSSLX509Certificate::New(certCtx, (IOpenSSLX509Certificate**)&res);
    *result = IX509Certificate::Probe(res);
    REFCOUNT_ADD(*result)

    return NOERROR;
}

AutoPtr<IOpenSSLKey> OpenSSLX509V3CertificateGenerator::FromPrivateKey(
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
        Logger::E("OpenSSLX509V3CertificateGenerator", "FromPrivateKey Unknown key format:%s", keyFormat.string());
        return NULL;
    }

    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(key)->GetEncoded((ArrayOf<Byte>**)&encoded);
    if (encoded == NULL) {
        // throw new InvalidKeyException("Key encoding is null");
        Logger::E("OpenSSLX509V3CertificateGenerator", "FromPrivateKey Key encoding is null:");
        return NULL;
    }

    Int64 key_info = 0;
    NativeCrypto::D2i_PKCS8_PRIV_KEY_INFO(encoded, &key_info);
    AutoPtr<IOpenSSLKey> res;
    COpenSSLKey::New(key_info, (IOpenSSLKey**)&res);
    return res;
}
//
AutoPtr<IOpenSSLKey> OpenSSLX509V3CertificateGenerator::WrapPrivateKey(
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
//
AutoPtr<IOpenSSLKey> OpenSSLX509V3CertificateGenerator::FromPublicKey(
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

} // namespace Conscrypt
} // namespace Org
