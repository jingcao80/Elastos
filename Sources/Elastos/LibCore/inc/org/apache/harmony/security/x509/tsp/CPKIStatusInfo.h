
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFO_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CPKIStatusInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CPKIStatusInfo)
{
public:
    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetFailInfo(
        /* [out] */ Int32 * pFailInfo);

    CARAPI GetStatus(
        /* [out] */ Org::Apache::Harmony::Security::X509::Tsp::PKIStatus * pStatus);

    CARAPI GetStatusString(
        /* [out] */ Elastos::Utility::IList ** ppList);

    CARAPI constructor(
        /* [in] */ Int32 pkiStatus,
        /* [in] */ Elastos::Utility::IList * pStatusString,
        /* [in] */ Int32 failInfo);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFO_H__
