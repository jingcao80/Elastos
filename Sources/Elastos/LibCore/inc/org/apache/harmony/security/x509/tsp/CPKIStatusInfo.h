
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFO_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CPKIStatusInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CPKIStatusInfo)
    , public Object
    , public IPKIStatusInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetFailInfo(
        /* [out] */ Int32* pFailInfo);

    CARAPI GetStatus(
        /* [out] */ PKIStatus* pStatus);

    CARAPI GetStatusString(
        /* [out] */ IList** ppList);

    CARAPI constructor(
        /* [in] */ Int32 pkiStatus,
        /* [in] */ IList* pStatusString,
        /* [in] */ Int32 failInfo);

private:
    // TODO: Add your private member variables here.
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFO_H__
