
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

    AutoPtr<IOpenSSLKey> publicKey = OpenSSLKey::FromPublicKey(pubkey);
    AutoPtr<IOpenSSLKey> privateKey = OpenSSLKey::FromPrivateKey(privkey);

    String hashalg = _hashalg;
    Int32 pkeyType = 0;
    Int64 pubkeycontext = 0;
    publicKey->GetPkeyContext(&pubkeycontext);
    NativeCrypto::EVP_PKEY_type(pubkeycontext, &pkeyType);

    Int32 pkeyType2 = 0;
    Int64 privkeycontext = 0;
    privateKey->GetPkeyContext(&privkeycontext);
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

} // namespace Conscrypt
} // namespace Org
