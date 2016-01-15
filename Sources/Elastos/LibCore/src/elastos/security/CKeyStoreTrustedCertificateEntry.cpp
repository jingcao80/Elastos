
#include "CKeyStoreTrustedCertificateEntry.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStoreTrustedCertificateEntry);
CAR_INTERFACE_IMPL_2(CKeyStoreTrustedCertificateEntry, Object, IKeyStoreTrustedCertificateEntry, IKeyStoreEntry);
ECode CKeyStoreTrustedCertificateEntry::GetTrustedCertificate(
    /* [out] */ Elastos::Security::Cert::ICertificate **cert)
{
    VALIDATE_NOT_NULL(cert)
    *cert = mTrustCertificate;
    REFCOUNT_ADD(*cert)
    return NOERROR;
}

ECode CKeyStoreTrustedCertificateEntry::ToString(
    /* [out] */ String *str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("Trusted certificate entry:\n");
    String cert;
    mTrustCertificate->ToString(&cert);
    (*str) += cert;
    return NOERROR;
}

ECode CKeyStoreTrustedCertificateEntry::constructor(
    /* [in] */ Elastos::Security::Cert::ICertificate *trustCertificate)
{
    if (trustCertificate == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mTrustCertificate = trustCertificate;
    return NOERROR;
}

}
}
