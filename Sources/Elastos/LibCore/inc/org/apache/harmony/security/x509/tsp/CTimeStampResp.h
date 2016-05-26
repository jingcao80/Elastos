
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPRESP_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPRESP_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CTimeStampResp.h"

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
        /* [out] */ String * pStr);

    CARAPI GetStatus(
        /* [out] */ Org::Apache::Harmony::Security::X509::Tsp::IPKIStatusInfo ** ppStatus);

    CARAPI GetTimeStampToken(
        /* [out] */ Org::Apache::Harmony::Security::Pkcs7::IContentInfo ** ppTimeStampToken);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::Tsp::IPKIStatusInfo * pStatus,
        /* [in] */ Org::Apache::Harmony::Security::Pkcs7::IContentInfo * pTimeStampToken);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPRESP_H__
