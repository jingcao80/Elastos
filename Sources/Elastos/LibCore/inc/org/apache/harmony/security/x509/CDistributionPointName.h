
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__

#include "_Org_Apache_Harmony_Security_X509_CDistributionPointName.h"

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
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralNames * pFullName);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X501::IName * pNameRelativeToCRLIssuer);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__
