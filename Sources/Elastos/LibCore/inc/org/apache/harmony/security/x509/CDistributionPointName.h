
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__

#include "_Org_Apache_Harmony_Security_X509_CDistributionPointName.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::X501::IName;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CDistributionPointName)
    , public Object
    , public IDistributionPointName
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ IGeneralNames* pFullName);

    CARAPI constructor(
        /* [in] */ IName* pNameRelativeToCRLIssuer);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__
