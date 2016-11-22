
#ifndef __ORG_CONSCRYPT_OPENSSLX509V3CERTIFICATEGENERATOR_H__
#define __ORG_CONSCRYPT_OPENSSLX509V3CERTIFICATEGENERATOR_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IDate;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Conscrypt {

class OpenSSLX509V3CertificateGenerator
    : public Object
{
public:

    static CARAPI Generate(
        /* [in] */ IPublicKey *pubkey,
        /* [in] */ IPrivateKey *privkey,
        /* [in] */ IBigInteger* serialNumber,
        /* [in] */ IDate* start,
        /* [in] */ IDate* end,
        /* [in] */ const String& subjectName,
        /* [in] */ const String& hashalg,
        /* [out] */ IX509Certificate** result);
private:
    static AutoPtr<IOpenSSLKey> FromPublicKey(
        /* [in] */ IPublicKey* key);

    static AutoPtr<IOpenSSLKey> FromPrivateKey(
        /* [in] */ IPrivateKey* key);

    static AutoPtr<IOpenSSLKey> WrapPrivateKey(
        /* [in] */ IPrivateKey* key);

};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509V3CERTIFICATEGENERATOR_H__

