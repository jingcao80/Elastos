
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPRESP_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPRESP_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CTimeStampResp.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Pkcs7::IContentInfo;
using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CTimeStampResp)
    , public Object
    , public ITimeStampResp
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetStatus(
        /* [out] */ IPKIStatusInfo** ppStatus);

    CARAPI GetTimeStampToken(
        /* [out] */ IContentInfo** ppTimeStampToken);

    CARAPI constructor(
        /* [in] */ IPKIStatusInfo* pStatus,
        /* [in] */ IContentInfo* pTimeStampToken);

private:
    // TODO: Add your private member variables here.
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPRESP_H__
