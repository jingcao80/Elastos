
#ifndef __ORG_CONSCRYPT_OPENSSLX509CRLENTRY_H__
#define __ORG_CONSCRYPT_OPENSSLX509CRLENTRY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ISet;
using Elastos::Utility::IDate;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Conscrypt {

class OpenSSLX509CRLEntry
    : public Object // public X509CRLEntry
    , public IOpenSSLX509CRLEntry
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI GetExtensionValue(
        /* [in] */ String oid,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** result);

    CARAPI GetRevocationDate(
        /* [out] */ IDate** result);

    CARAPI HasExtensions(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509CRLENTRY_H__