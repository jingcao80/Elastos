
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCRLDISTRIBUTIONPOINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCRLDISTRIBUTIONPOINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CCRLDistributionPoints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCRLDistributionPoints)
    , public Object
    , public ICRLDistributionPoints
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncode);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValueEx(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCRLDISTRIBUTIONPOINTS_H__
