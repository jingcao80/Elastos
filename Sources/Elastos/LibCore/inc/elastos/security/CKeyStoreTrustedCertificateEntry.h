
#ifndef __ELASTOS_SECURITY_CKEYSTORETRUSTEDCERTIFICATEENTRY_H__
#define __ELASTOS_SECURITY_CKEYSTORETRUSTEDCERTIFICATEENTRY_H__

#include "_Elastos_Security_CKeyStoreTrustedCertificateEntry.h"
#include "Object.h"

namespace Elastos {
namespace Security {

CarClass(CKeyStoreTrustedCertificateEntry)
    , public Object
    , public IKeyStoreTrustedCertificateEntry
    , public IKeyStoreEntry
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetTrustedCertificate(
        /* [out] */ Elastos::Security::Cert::ICertificate **cert);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ Elastos::Security::Cert::ICertificate *trustCertificate);

private:
    // Store trusted Certificate
    AutoPtr<Elastos::Security::Cert::ICertificate> mTrustCertificate;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYSTORETRUSTEDCERTIFICATEENTRY_H__
