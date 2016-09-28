
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CBASICCONSTRAINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CBASICCONSTRAINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CBasicConstraints.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CBasicConstraints)
    , public Object
    , public IBasicConstraints
    , public IExtensionValue
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCa(
        /* [out] */ Boolean* ca);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI GetPathLenConstraint(
        /* [out] */ Int32* pPathLenConstraint);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CBASICCONSTRAINTS_H__
