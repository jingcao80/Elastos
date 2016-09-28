
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTLIST_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTLIST_H__

#include "_Org_Apache_Harmony_Security_X509_CTBSCertList.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::X501::IName;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CTBSCertList)
    , public Object
    , public ITBSCertList
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetVersion(
        /* [out] */ Int32* pVer);

    CARAPI GetSignature(
        /* [out] */ IAlgorithmIdentifier** ppSignature);

    CARAPI GetIssuer(
        /* [out] */ IName** ppIssuer);

    CARAPI GetThisUpdate(
        /* [out] */ IDate** ppDate);

    CARAPI GetNextUpdate(
        /* [out] */ IDate** ppDate);

    CARAPI GetRevokedCertificates(
        /* [out] */ IList** ppCertificates);

    CARAPI GetCrlExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface* pRc,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pHashCode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTLIST_H__
