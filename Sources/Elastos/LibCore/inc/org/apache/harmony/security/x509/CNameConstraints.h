
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CNAMECONSTRAINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CNAMECONSTRAINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CNameConstraints.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CNameConstraints)
    , public Object
    , public INameConstraints
    , public IExtensionValue
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI IsAcceptable(
        /* [in] */ IX509Certificate* pCert,
        /* [out] */ Boolean* pIsAcceptable);

    CARAPI IsAcceptableEx(
        /* [in] */ IList* pNames,
        /* [out] */ Boolean* pIsAcceptable);

    CARAPI constructor(
        /* [in] */ IGeneralSubtrees* pPermittedSubtrees,
        /* [in] */ IGeneralSubtrees* pExcludedSubtrees);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CNAMECONSTRAINTS_H__
